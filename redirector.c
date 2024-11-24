/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirector.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 12:29:24 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/24 17:36:22 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	clear_output_redirect(const char *output_redirect, int *last_exit_status)
{
	int	fd_out;

	if (output_redirect)
	{
		fd_out = open(output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd_out == -1)
		{
			perror("Failed to create or truncate output file");
			*last_exit_status = 1;
			return (1);
		}
		close(fd_out);
	}
	return (0);
}

int	handle_input_redirection(const char *input_redirect, int *saved_stdin,
		int *last_exit_status)
{
	int	fd_in;

	if (input_redirect)
	{
		fd_in = open(input_redirect, O_RDONLY);
		if (fd_in == -1)
		{
			perror("open failed for input redirection");
			*last_exit_status = 1;
			return (1);
		}
		*saved_stdin = dup(STDIN_FILENO);
		if (*saved_stdin == -1 || dup2(fd_in, STDIN_FILENO) == -1)
		{
			perror("dup failed for input redirection");
			close(fd_in);
			*last_exit_status = 1;
			return (1);
		}
		close(fd_in);
	}
	return (0);
}

int	handle_output_redirection(const char *output_redirect, int append_mode,
		int *saved_stdout, int *last_exit_status)
{
	int	fd_out;

	if (!output_redirect)
	{
		return (0);
	}
	if (append_mode)
	{
		fd_out = open(output_redirect, O_WRONLY | O_CREAT | O_APPEND, 0644);
	}
	else
	{
		fd_out = open(output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	}
	if (fd_out == -1)
	{
		perror("open failed for output redirection");
		*last_exit_status = 1;
		return (1);
	}
	*saved_stdout = dup(STDOUT_FILENO);
	if (*saved_stdout == -1 || dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("dup failed for output redirection");
		close(fd_out);
		*last_exit_status = 1;
		return (1);
	}
	close(fd_out);
	return (0);
}

void	handle_redirections(t_token **current_token, t_command *current_command,
		t_memories *memories, int append_mode)
{
	char	*redirect;

	if (!current_command)
	{
		printf("Error: current_command is NULL while handling redirections.\n");
		return ;
	}
	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		redirect = strdup((*current_token)->value);
		if (!redirect)
		{
			printf("Error: Failed to allocate memory for redirect.\n");
			return ;
		}
		add_memory(memories, redirect);
		if (append_mode)
		{
			current_command->output_redirect = redirect;
			current_command->append_mode = 1;
		}
		else
		{
			current_command->output_redirect = redirect;
		}
	}
	else
	{
		printf("Error: Expected a filename after redirection token.\n");
	}
}

void	restore_redirections(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1)
			perror("Failed to restore stdin");
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		if (dup2(saved_stdout, STDOUT_FILENO) == -1)
			perror("Failed to restore stdout");
		close(saved_stdout);
	}
}

void	handle_all_redirections(t_token **current_token,
			t_command *current_command, t_memories *memories)
{
	int	append_mode;

	append_mode = 0;
	if ((*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT)
	{
		append_mode = 1;
	}
	else
	{
	}
	handle_redirections(current_token, current_command, memories, append_mode);
}
