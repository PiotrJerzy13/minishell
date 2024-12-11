/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 15:49:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/11 00:43:35 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	write_heredoc_to_pipe(t_heredoc_node *node, int pipe_fd)
{
	while (node)
	{
		if (write(pipe_fd, node->line, ft_strlen(node->line)) == -1
			|| write(pipe_fd, "\n", 1) == -1)
		{
			perror("Write to heredoc pipe failed");
			exit(1);
		}
		node = node->next;
	}
	close(pipe_fd);
	exit(0);
}

int	handle_heredoc2(t_command *current_command, int *in_fd,
	int *last_exit_status)
{
	int		pipe_fd[2];
	pid_t	pid;

	if (pipe(pipe_fd) == -1)
	{
		perror("Pipe creation failed for heredoc");
		*last_exit_status = 1;
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("Fork failed for heredoc");
		*last_exit_status = 1;
		return (-1);
	}
	if (pid == 0)
		write_heredoc_to_pipe(current_command->heredoc_list, pipe_fd[1]);
	close(pipe_fd[1]);
	if (*in_fd != STDIN_FILENO)
		close(*in_fd);
	*in_fd = pipe_fd[0];
	wait(NULL);
	return (0);
}

int	handle_input_redirection2(t_command *current_command, int in_fd)
{
	int	fd_in;

	fd_in = open(current_command->input_redirect, O_RDONLY);
	if (current_command->input_redirect)
	{
		if (fd_in == -1)
		{
			perror("Failed to open input file");
			return (-1);
		}
		if (dup2(fd_in, STDIN_FILENO) == -1)
		{
			perror("dup2 failed for input redirection");
			close(fd_in);
			return (-1);
		}
		close(fd_in);
	}
	else if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
			return (-1);
		close(in_fd);
	}
	return (0);
}

int	handle_output_redirection2(t_command *current_command)
{
	int	fd_out;

	if (current_command->output_redirect)
	{
		if (current_command->append_mode)
		{
			fd_out = open(current_command->output_redirect, O_WRONLY
					| O_CREAT | O_APPEND, 0644);
		}
		else
			fd_out = open(current_command->output_redirect, O_WRONLY
					| O_CREAT | O_TRUNC, 0644);
		if (fd_out == -1)
		{
			return (-1);
		}
		if (dup2(fd_out, STDOUT_FILENO) == -1)
		{
			perror("dup2 failed for output redirection");
			close(fd_out);
			return (-1);
		}
		close(fd_out);
	}
	return (0);
}
