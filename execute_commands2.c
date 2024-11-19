/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_commands2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:50:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/19 15:09:54 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	setup_pipe_if_needed(t_command *command, int *pipefd, int *last_exit_status)
{
	if (!is_last_command(command))
	{
		if (setup_pipes(pipefd, last_exit_status) == -1)
			return (-1);
	}
	return (0);
}

void	handle_parent_cleanup(int in_fd, int *pipefd, t_command *command)
{
	if (in_fd != STDIN_FILENO)
		close(in_fd);
	if (!is_last_command(command))
		close(pipefd[1]);
}

void	wait_for_children(int *last_exit_status)
{
	int	status;

	while (wait(&status) > 0)
	{
		if (WIFEXITED(status))
			*last_exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			*last_exit_status = 128 + WTERMSIG(status);
	}
}

void	handle_command_execution(t_command *command, t_exec_context *context)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		*(context->last_exit_status) = 1;
		return ;
	}
	else if (pid == 0)
	{
		setup_child_redirections(context->in_fd, context->pipefd,
			is_last_command(command));
		execute_command(command, context->environment,
			context->last_exit_status);
	}
	else
	{
		handle_parent_cleanup(context->in_fd, context->pipefd, command);
	}
}

int	prepare_next_command(int *pipefd, t_command *command)
{
	if (is_last_command(command))
		return (STDIN_FILENO);
	else
		return (pipefd[0]);
}
