/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/15 18:12:49 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execute_with_pipe_and_redirect(t_command *command_list,
	int *last_exit_status, t_env *environment)
{
	t_command	*current_command = command_list;
	int	pipefd[2], status;
	pid_t		pid;
	char		*exec_path = NULL;
	char		**env_array;
	int			in_fd = STDIN_FILENO;
	char		*output_buffer = malloc(4096);
	size_t		buffer_size = 0;

	if (!output_buffer)
	{
		perror("malloc failed for output buffer");
		*last_exit_status = 1;
		return ;
	}
	while (current_command)
	{
		int is_last_command = (current_command->next == NULL);
        if (!is_last_command && pipe(pipefd) == -1)
		{
			perror("pipe failed");
			*last_exit_status = 1;
			free(output_buffer);
			return ;
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork failed");
			*last_exit_status = 1;
			free(output_buffer);
			return ;
		}
		else if (pid == 0) {
            // In child process
            if (in_fd != STDIN_FILENO) {
                if (dup2(in_fd, STDIN_FILENO) == -1) {
                    perror("dup2 failed for stdin redirection");
                    exit(1);
                }
                close(in_fd);
            }

            if (!is_last_command) {
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("dup2 failed for stdout redirection");
                    exit(1);
                }
                close(pipefd[1]);
            } else {
                // Capture the output in buffer instead of direct file redirection
                int mem_pipe[2];
                if (pipe(mem_pipe) == -1) {
                    perror("pipe for memory buffer failed");
                    exit(1);
                }
                if (dup2(mem_pipe[1], STDOUT_FILENO) == -1) {
                    perror("dup2 failed for memory buffer");
                    close(mem_pipe[0]);
                    close(mem_pipe[1]);
                    exit(1);
                }
                close(mem_pipe[1]);
                close(pipefd[0]);

                // Read from memory buffer into output_buffer
                buffer_size = read(mem_pipe[0], output_buffer, 4096);
                close(mem_pipe[0]);
            }

            env_array = env_to_char_array(environment);
            exec_path = find_executable_path(current_command->command);

            if (!exec_path) {
                fprintf(stderr, "minishell: %s: command not found\n", current_command->command);
                *last_exit_status = 127;
                free_env_array(env_array);
                exit(127);
            }
            execve(exec_path, current_command->args, env_array);
            perror("execve failed");
            free(exec_path);
            free_env_array(env_array);
            exit(1);
        } else {
            // In parent process
            close(pipefd[1]);  // Close write end of pipe in parent

            if (in_fd != STDIN_FILENO) {
                close(in_fd);
            }

            in_fd = pipefd[0];  // Set up pipe read end for next command

            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                *last_exit_status = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                *last_exit_status = 128 + WTERMSIG(status);
            }

            current_command = current_command->next;
        }
    }

    // Write buffer to file if output redirection exists
    t_command *last_command = command_list;
    while (last_command->next) last_command = last_command->next;

    if (last_command->output_redirect) {
        int fd_out = open(last_command->output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1) {
            perror("open failed for output redirection file");
            *last_exit_status = 1;
        } else {
            write(fd_out, output_buffer, buffer_size);
            close(fd_out);
        }
    }

    free(output_buffer);
    printf("All processes in special execution completed\n");
}
void execute_pipeline(t_command *command_list, int *last_exit_status, t_env *environment) {
    t_command *current_command = command_list;
    int requires_special_handling = 0;

    // Detect if there's a pipe followed by an output redirection in sequence
    while (current_command) {
        if (current_command->is_pipe && current_command->next && current_command->next->output_redirect) {
            requires_special_handling = 1;
            break;
        }
        current_command = current_command->next;
    }

    // Choose the appropriate function based on detection
    if (requires_special_handling) {
        printf("Special case detected: using execute_with_pipe_and_redirect\n");
        execute_with_pipe_and_redirect(command_list, last_exit_status, environment);
    } else {
        printf("Using standard execute_commands\n");
        execute_commands(command_list, last_exit_status, environment);
    }
}
int main(int argc, char **argv, char **env) {
    t_command *command_list;
    t_env environment;
    char *input;
    t_memories memories;
    t_token *token_list;
    char *line;
    int last_exit_status = 0;

    (void)argc;
    (void)argv;
    command_list = NULL;
    token_list = NULL;

    if (initialize_shell() == -1) {
        fprintf(stderr, "Failed to initialize shell environment\n");
        return (EXIT_FAILURE);
    }
    signal(SIGQUIT, SIG_IGN);
    init_memories(&memories, &environment, 10);
    copy_environment_to_struct(env, &environment, &memories);

    while (1) {
        if (isatty(fileno(stdin))) {
            input = readline("minishell> ");
        } else {
            line = get_next_line(fileno(stdin));
            if (line == NULL)
                break;
            input = ft_strtrim(line, "\n");
            free(line);
        }
        if (input == NULL) {
            if (isatty(fileno(stdin)))
                printf("exit\n");
            break;
        }
        if (*input) {
            add_history(input);
            tokenize_input(input, &token_list, &memories, &environment, &last_exit_status);
            parse_input_to_commands(token_list, &command_list, &memories);

            if (command_list) {
                if (handle_builtin(command_list, &environment, &memories, &last_exit_status) == 1) {
                    // Builtin handled directly
                } else {
                    // Execute commands based on condition
                    execute_pipeline(command_list, &last_exit_status, &environment);
                }
            }
        }
        free(input);
        input = NULL;
        command_list = NULL;
        token_list = NULL;
    }
    free_all_memories(&memories);
    return (last_exit_status);
}