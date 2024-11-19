/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:34:40 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/19 15:29:32 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	setup_child_redirections(int in_fd, int *pipefd, int is_last_command)
{
	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 failed for input redirection");
			exit(1);
		}
		close(in_fd);
	}
	if (!is_last_command)
	{
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2 failed for output redirection");
			exit(1);
		}
		close(pipefd[1]);
	}
	if (!is_last_command)
		close(pipefd[0]);
}

void	execute_command(t_command *command,
	t_env *environment, int *last_exit_status)
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
	execve(exec_path, command->args, env_array);
	perror("[ERROR] execve failed");
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

void	execute_commands(t_command *command_list,
		int *last_exit, t_env *environment)
{
	t_command		*current_command;
	t_exec_context	context;
	int				pipefd[2];
	int				in_fd;

	current_command = command_list;
	in_fd = STDIN_FILENO;
	context.last_exit_status = last_exit;
	context.environment = environment;
	while (current_command)
	{
		in_fd = handle_heredoc_if_needed(current_command, in_fd, last_exit);
		if (in_fd == -1)
			return ;
		if (setup_pipe_if_needed(current_command, pipefd, last_exit) == -1)
			return ;
		context.in_fd = in_fd;
		context.pipefd = pipefd;
		handle_command_execution(current_command, &context);
		in_fd = prepare_next_command(pipefd, current_command);
		current_command = current_command->next;
	}
	wait_for_children(last_exit);
}
