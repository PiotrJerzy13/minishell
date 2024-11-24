/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:34:40 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/24 14:39:57 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	detect_circular_dependency(const char *input_redirect,
	const char *output_redirect)
{
	if (input_redirect && output_redirect && strcmp(input_redirect,
			output_redirect) == 0)
	{
		fprintf(stderr, "Error: Input and output file: %s\n", input_redirect);
		return (1);
	}
	return (0);
}

void	exe_command(t_command *command, t_env *environment,
	int *last_exit_status)
{
	char	**env_array;
	char	*exec_path;

	env_array = env_to_char_array(environment);
	exec_path = find_executable_path(command->command);
	if (!exec_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", command->command);
		*last_exit_status = 127;
		free_env_array(env_array);
		exit(127);
	}
	printf("Executing command: %s\n", exec_path);
	printf("Arguments:\n");
	for (int i = 0; command->args[i]; i++)
	{
		printf("  [%d]: %s\n", i, command->args[i]);
	}
	execve(exec_path, command->args, env_array);
	perror("execve failed");
	free_env_array(env_array);
	exit(1);
}

int	is_last_command(t_command *command)
{
	return (command->next == NULL);
}

int	handle_heredoc_if_needed(t_command *command, int in_fd,
	int *last_exit_status)
{
	if (command->heredoc_list)
	{
		in_fd = handle_heredoc_redirection(command, last_exit_status);
	}
	return (in_fd);
}

int	prepare_next_command(int *pipefd, t_command *command)
{
	if (is_last_command(command))
		return (STDIN_FILENO);
	else
		return (pipefd[0]);
}

void	add_argument_to_command(t_token *current_token, t_command *current_command,
	t_memories *memories, int *arg_count)
{
	current_command->args[*arg_count] = strdup(current_token->value);
	add_memory(memories, current_command->args[*arg_count]);
	printf("Argument added: %s (total arguments: %d)\n",
		current_command->args[*arg_count], *arg_count + 1);
	(*arg_count)++;
}

void execute_commands(t_command *command_list, int *last_exit_status, t_env *environment)
{
	t_command	*current_command = command_list;
	int			in_fd = STDIN_FILENO, pipe_fd[2];
	pid_t		pid;

	while (current_command)
	{
		if (current_command->heredoc_list)
		{
			if (pipe(pipe_fd) == -1)
			{
				perror("Pipe creation failed for heredoc");
				*last_exit_status = 1;
				return ;
			}
			pid = fork();
			if (pid == -1)
			{
				perror("Fork failed for heredoc writer");
				*last_exit_status = 1;
				return ;
			}
			if (pid == 0)
			{
				close(pipe_fd[0]);
				t_heredoc_node *node = current_command->heredoc_list;
				while (node)
				{
					if (write(pipe_fd[1], node->line, strlen(node->line)) == -1)
					{
						perror("Write to heredoc pipe failed");
						exit(1);
					}
					if (write(pipe_fd[1], "\n", 1) == -1)
					{
						perror("Write newline to heredoc pipe failed");
						exit(1);
					}
					node = node->next;
				}
				close(pipe_fd[1]);
				exit(0);
			}
			close(pipe_fd[1]);
			if (in_fd != STDIN_FILENO)
				close(in_fd);
			in_fd = pipe_fd[0];
			wait(NULL);
		}
		if (current_command->next)
		{
			if (pipe(pipe_fd) == -1)
			{
				perror("Pipe creation failed");
				*last_exit_status = 1;
				return ;
			}
		}
		pid = fork();
		if (pid == -1)
		{
			perror("Fork failed");
			*last_exit_status = 1;
			return ;
		}
		if (pid == 0)
		{
			if (current_command->input_redirect)
			{
				int fd_in = open(current_command->input_redirect, O_RDONLY);
				if (fd_in == -1)
				{
					perror("Failed to open input file");
					exit(1);
				}
				if (dup2(fd_in, STDIN_FILENO) == -1)
				{
					perror("dup2 failed for input redirection");
					close(fd_in);
					exit(1);
				}
				close(fd_in);
			}
			else if (in_fd != STDIN_FILENO)
			{
				if (dup2(in_fd, STDIN_FILENO) == -1)
				{
					perror("dup2 failed for heredoc or pipe input");
					exit(1);
				}
				close(in_fd);
			}
			if (current_command->output_redirect)
			{
				int fd_out = open(current_command->output_redirect,
						current_command->append_mode ? (O_WRONLY | O_CREAT | O_APPEND)
						: (O_WRONLY | O_CREAT | O_TRUNC), 0644);
				if (fd_out == -1)
				{
					perror("Failed to open output file");
					exit(1);
				}
				if (dup2(fd_out, STDOUT_FILENO) == -1)
				{
					perror("dup2 failed for output redirection");
					close(fd_out);
					exit(1);
				}
				close(fd_out);
			}
			else if (current_command->next)
			{
				if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
				{
					perror("dup2 failed for pipe output");
					exit(1);
				}
				close(pipe_fd[1]);
			}
			if (current_command->next)
				close(pipe_fd[0]);
			exe_command(current_command, environment, last_exit_status);
			exit(*last_exit_status);
		}
		if (in_fd != STDIN_FILENO)
			close(in_fd);
		if (current_command->next)
			close(pipe_fd[1]);
		in_fd = pipe_fd[0];
		current_command = current_command->next;
	}
	int status;
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status))
		{
			*last_exit_status = WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status))
		{
			*last_exit_status = 128 + WTERMSIG(status);
		}
	}
}
