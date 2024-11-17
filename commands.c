/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/17 16:17:27 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_command_node(t_command **head, t_command *new_command)
{
	t_command	*current;

	current = *head;
	if (!current)
	{
		*head = new_command;
		return ;
	}
	while (current->next)
		current = current->next;
	current->next = new_command;
}

void	parse_input_to_commands(t_token *token_list, t_command **command_list,
	t_memories *memories)
{
	t_command	*current_command;
	t_token		*current_token;
	int			arg_count;

	current_token = token_list;
	arg_count = 0;
	current_command = NULL;
	while (current_token)
	{
		if (current_command == NULL && current_token->type == TOKEN_COMMAND)
		{
			current_command = init_command_node(memories);
			add_command_node(command_list, current_command);
			current_command->args = malloc(sizeof(char *) * 10);
			add_memory(memories, current_command->args);
			if (strncmp(current_token->value, "./", 2) == 0)
			{
				current_command->command = strdup(current_token->value);
				add_memory(memories, current_command->command);
			}
			else
			{
				current_command->command = strdup(current_token->value);
				add_memory(memories, current_command->command);
			}
			current_command->args[arg_count++] = strdup(current_token->value);
			add_memory(memories, current_command->args[arg_count - 1]);
		}
		else if ((current_token->type == TOKEN_ARGUMENT
				|| current_token->type == TOKEN_COMMAND) && current_command)
		{
			current_command->args[arg_count++] = strdup(current_token->value);
			add_memory(memories, current_command->args[arg_count - 1]);
		}
		else if (current_token->type == TOKEN_HEREDOC && current_command)
		{
    		printf("[DEBUG] Handling heredoc for command: %s\n", current_command->command);
    		current_token = current_token->next; // Move to the delimiter token
    		if (current_token && current_token->type == TOKEN_FILENAME)
    		{
        		current_command->heredoc_list = NULL;
        		if (collect_heredoc_input(current_token->value, &current_command->heredoc_list) == -1)
        		{
            		fprintf(stderr, "Error collecting heredoc input\n");
            		return;
        		}
        	printf("[DEBUG] Heredoc input collected for command: %s\n", current_command->command);
    		}
		}
		else if (current_token->type == TOKEN_OUTPUT_REDIRECT
			&& current_command)
		{
			current_token = current_token->next;
			if (current_token && current_token->type == TOKEN_FILENAME)
			{
				current_command->output_redirect = strdup(current_token->value);
				add_memory(memories, current_command->output_redirect);
			}
		}
		else if (current_token->type == TOKEN_APPEND_OUTPUT_REDIRECT
			&& current_command)
		{
			current_token = current_token->next;
			if (current_token && current_token->type == TOKEN_FILENAME)
			{
				current_command->output_redirect = strdup(current_token->value);
				add_memory(memories, current_command->output_redirect);
				current_command->append_mode = 1;
			}
		}
		else if (current_token->type == TOKEN_INPUT_REDIRECT && current_command)
		{
			current_token = current_token->next;
			if (current_token && current_token->type == TOKEN_FILENAME)
			{
				current_command->input_redirect = strdup(current_token->value);
				add_memory(memories, current_command->input_redirect);
			}
		}
		else if (current_token->type == TOKEN_PIPE && current_command)
		{
			current_command->is_pipe = 1;
			current_command->args[arg_count] = NULL;
			current_command = NULL;
			arg_count = 0;
		}
		current_token = current_token->next;
	}
	if (current_command && arg_count < 10)
	{
		current_command->args[arg_count] = NULL;
	}
}

char	*find_executable_path(const char *command)
{
	char	*path;
	char	*dir;
	char	*path_env;
	char	*full_path;
	size_t	len;

	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	path = strdup(path_env);
	dir = strtok(path, ":");
	while (dir != NULL)
	{
		len = strlen(dir) + strlen(command) + 2;
		full_path = malloc(len);
		if (!full_path)
		{
			free(path);
			return (NULL);
		}
		snprintf(full_path, len, "%s/%s", dir, command);
		if (access(full_path, X_OK) == 0)
		{
			free(path);
			return (full_path);
		}
		free(full_path);
		dir = strtok(NULL, ":");
	}
	free(path);
	return (NULL);
}

// void	execute_commands(t_command *command_list, int *last_exit_status,
// 	t_env *environment)
// {
// 	t_command	*current_command;
// 	int			pipefd[2];
// 	int			in_fd;
// 	pid_t		pid;
// 	int			status;
// 	int			builtin_status;
// 	char		*exec_path;
// 	char		**env_array;
// 	int			is_last_command;
// 	int			fd_out;

// 	in_fd = STDIN_FILENO;
// 	exec_path = NULL;
// 	current_command = command_list;
// 	while (current_command)
// 	{
// 		printf("Processing command: %s\n", current_command->command);
// 		builtin_status = handle_builtin(current_command, environment, NULL,
// 				last_exit_status);
// 		printf("Builtin status: %d\n", builtin_status);
// 		if (builtin_status == 1)
// 		{
// 			printf("Executed builtin command: %s\n", current_command->command);
// 			current_command = current_command->next;
// 			continue ;
// 		}
// 		else if (builtin_status == -1)
// 		{
// 			printf("Exit command detected\n");
// 			*last_exit_status = 0;
// 			break ;
// 		}
// 		if (current_command->input_redirect && current_command->output_redirect
// 			&& is_same_file(current_command->input_redirect,
// 				current_command->output_redirect))
// 		{
// 			printf("Input file. Clearing output file: %s\n",
// 				current_command->output_redirect);
// 			fd_out = open(current_command->output_redirect,
// 					O_WRONLY | O_TRUNC, 0644);
// 			if (fd_out == -1)
// 			{
// 				perror("open failed for clearing output file");
// 				*last_exit_status = 1;
// 				return ;
// 			}
// 			close(fd_out);
// 		}
// 		is_last_command = (current_command->next == NULL);
// 		if (!is_last_command && pipe(pipefd) == -1)
// 		{
// 			perror("pipe failed");
// 			*last_exit_status = 1;
// 			return ;
// 		}
// 		if (!is_last_command)
// 		{
// 			printf("Pipe created between commands\n");
// 		}
// 		pid = fork();
// 		if (pid == -1)
// 		{
// 			perror("fork failed");
// 			*last_exit_status = 1;
// 			return ;
// 		}
// 		else if (pid == 0)
// 		{
// 			printf("In child process for command: %s\n",
// 				current_command->command);
// 			if (in_fd != STDIN_FILENO)
// 			{
// 				printf("Redirecting stdin\n");
// 				if (dup2(in_fd, STDIN_FILENO) == -1)
// 				{
// 					perror("dup2 failed for input redirection");
// 					exit(1);
// 				}
// 				close(in_fd);
// 			}
// 			if (!is_last_command)
// 			{
// 				printf("Redirecting stdout to pipe\n");
// 				if (dup2(pipefd[1], STDOUT_FILENO) == -1)
// 				{
// 					perror("dup2 failed for output redirection");
// 					exit(1);
// 				}
// 				close(pipefd[1]);
// 			}
// 			else if (current_command->output_redirect)
// 			{
// 				fd_out = open(current_command->output_redirect,
// 						O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 				if (fd_out == -1)
// 				{
// 					perror("open failed for output redirection");
// 					exit(1);
// 				}
// 				printf("Redirecting stdout to output file: %s\n",
// 					current_command->output_redirect);
// 				if (dup2(fd_out, STDOUT_FILENO) == -1)
// 				{
// 					perror("dup2 failed for output redirection to file");
// 					close(fd_out);
// 					exit(1);
// 				}
// 				close(fd_out);
// 			}
// 			if (!is_last_command)
// 			{
// 				close(pipefd[0]);
// 			}
// 			env_array = env_to_char_array(environment);
// 			if (strchr(current_command->command, '/'))
// 			{
// 				exec_path = strdup(current_command->command);
// 			}
// 			else
// 			{
// 				exec_path = find_executable_path(current_command->command);
// 				if (!exec_path)
// 				{
// 					fprintf(stderr, "minishell: %s: command not found\n",
// 						current_command->command);
// 					*last_exit_status = 127;
// 					free_env_array(env_array);
// 					exit(127);
// 				}
// 			}
// 			printf("Executing command: %s\n", exec_path);
// 			execve(exec_path, current_command->args, env_array);
// 			perror("execve failed");
// 			free(exec_path);
// 			free_env_array(env_array);
// 			exit(1);
// 		}
// 		else
// 		{
// 			printf("In parent process, forked child PID: %d\n", pid);
// 			if (in_fd != STDIN_FILENO)
// 			{
// 				close(in_fd);
// 				printf("Closed previous input file descriptor\n");
// 			}
// 			if (!is_last_command)
// 			{
// 				close(pipefd[1]);
// 				printf("Closed unused write end of pipe\n");
// 			}
// 			in_fd = pipefd[0];
// 			printf("Setting up in_fd for next command\n");
// 			current_command = current_command->next;
// 			if (exec_path)
// 			{
// 				free(exec_path);
// 				exec_path = NULL;
// 			}
// 		}
// 	}
// 	printf("Waiting for child processes to complete\n");
// 	while (wait(&status) > 0)
// 	{
// 		if (WIFEXITED(status))
// 		{
// 			*last_exit_status = WEXITSTATUS(status);
// 			printf("Child exited with status: %d\n", *last_exit_status);
// 		}
// 		else if (WIFSIGNALED(status))
// 		{
// 			*last_exit_status = 128 + WTERMSIG(status);
// 			printf("Child terminated by signal: %d\n", WTERMSIG(status));
// 		}
// 	}
// }

void execute_commands(t_command *command_list, int *last_exit_status, t_env *environment)
{
    t_command   *current_command;
    int         pipefd[2];
    int         in_fd;
    pid_t       pid;
    int         status;
    char        *exec_path;
    char        **env_array;
    int         is_last_command;
    int         heredoc_pipe[2];

    in_fd = STDIN_FILENO;
    current_command = command_list;

    while (current_command)
    {
        printf("Processing command: %s\n", current_command->command);

        // Handle heredoc if present
        if (current_command->heredoc_list)
        {
            printf("[DEBUG] Handling heredoc for command: %s\n", current_command->command);
            if (pipe(heredoc_pipe) == -1)
            {
                perror("[ERROR] Pipe for heredoc failed");
                *last_exit_status = 1;
                return;
            }

            t_heredoc_node *current_node = current_command->heredoc_list;
            while (current_node)
            {
                write(heredoc_pipe[1], current_node->line, strlen(current_node->line));
                write(heredoc_pipe[1], "\n", 1);
                printf("[DEBUG] Wrote heredoc line to pipe: %s\n", current_node->line);
                current_node = current_node->next;
            }
            close(heredoc_pipe[1]); // Close write-end after writing all heredoc lines
            in_fd = heredoc_pipe[0]; // Set input file descriptor to heredoc pipe's read-end
        }

        is_last_command = (current_command->next == NULL);
        if (!is_last_command && pipe(pipefd) == -1)
        {
            perror("pipe failed");
            *last_exit_status = 1;
            return;
        }

        pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            *last_exit_status = 1;
            return;
        }
        else if (pid == 0) // Child process
        {
            printf("[DEBUG] In child process for command: %s\n", current_command->command);

            if (in_fd != STDIN_FILENO)
            {
                printf("[DEBUG] Redirecting stdin\n");
                if (dup2(in_fd, STDIN_FILENO) == -1)
                {
                    perror("dup2 failed for input redirection");
                    exit(1);
                }
                close(in_fd);
            }

            if (!is_last_command)
            {
                printf("[DEBUG] Redirecting stdout to pipe\n");
                if (dup2(pipefd[1], STDOUT_FILENO) == -1)
                {
                    perror("dup2 failed for output redirection");
                    exit(1);
                }
                close(pipefd[1]);
            }

            // Close unused pipe ends in child
            if (!is_last_command)
                close(pipefd[0]);

            env_array = env_to_char_array(environment);
            exec_path = find_executable_path(current_command->command);
            if (!exec_path)
            {
                fprintf(stderr, "minishell: %s: command not found\n", current_command->command);
                *last_exit_status = 127;
                free_env_array(env_array);
                exit(127);
            }

            printf("[DEBUG] Executing command: %s\n", exec_path);
            execve(exec_path, current_command->args, env_array);

            perror("[ERROR] execve failed");
            exit(1);
        }
        else // Parent process
        {
            printf("[DEBUG] In parent process, forked child PID: %d\n", pid);

            if (in_fd != STDIN_FILENO)
                close(in_fd);

            if (!is_last_command)
                close(pipefd[1]);

            in_fd = is_last_command ? STDIN_FILENO : pipefd[0];
            current_command = current_command->next;
        }
    }

    printf("[DEBUG] Waiting for child processes to complete\n");
    while (wait(&status) > 0)
    {
        if (WIFEXITED(status))
        {
            *last_exit_status = WEXITSTATUS(status);
            printf("[DEBUG] Child exited with status: %d\n", *last_exit_status);
        }
        else if (WIFSIGNALED(status))
        {
            *last_exit_status = 128 + WTERMSIG(status);
            printf("[DEBUG] Child terminated by signal: %d\n", WTERMSIG(status));
        }
    }
}
