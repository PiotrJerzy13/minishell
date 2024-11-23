/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:50:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:41:16 by pwojnaro         ###   ########.fr       */
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

void	handle_pipe(t_command **current_command, int *arg_count)
{
	if (*current_command)
	{
		(*current_command)->is_pipe = 1;
		(*current_command)->args[*arg_count] = NULL;
		*current_command = NULL;
		*arg_count = 0;
	}
}
