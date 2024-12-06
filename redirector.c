/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirector.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 12:29:24 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/06 16:00:44 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	clear_output_redirect(const char *output_redirect, int *last_exit_status)
{
	int	fd_out;

	if (output_redirect)
	{
		printf("DEBUG: Clearing output redirection: %s\n", output_redirect);
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
	printf("DEBUG: Output redirection detected: %s\n", output_redirect);
	printf("DEBUG: Append mode: %d\n", append_mode);
	if (append_mode)
	{
		fd_out = open(output_redirect, O_WRONLY | O_CREAT | O_APPEND, 0644);
		printf("DEBUG: File opened in append mode: %s\n", output_redirect);
	}
	else
	{
		fd_out = open(output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		printf("DEBUG: File opened in overwrite mode: %s\n", output_redirect);
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

int	handle_redirections(t_token **current_token, t_command *current_command,
	t_memories *memories)
{
	char	*redirect;

	if (!current_command)
	{
		fprintf(stderr, "Error: No current command.\n");
		return (-1);
	}
	printf("DEBUG: Token type being processed: %d\n", (*current_token)->type);
	if ((*current_token)->type == TOKEN_INPUT_REDIRECT)
	{
		printf("DEBUG: Detected input redirection.\n");
		*current_token = (*current_token)->next;
		if (*current_token && (*current_token)->type == TOKEN_FILENAME)
		{
			redirect = strdup((*current_token)->value);
			if (!redirect)
			{
				fprintf(stderr, "Error: Failed to allocate memory.\n");
				return (-1);
			}
			add_memory(memories, redirect);
			printf("DEBUG: Handling input redirection: %s\n", redirect);
			current_command->input_redirect = redirect;
		}
		else
		{
			fprintf(stderr, "Error: Expected filename.\n");
			return (-1);
		}
	}
	else if ((*current_token)->type == TOKEN_OUTPUT_REDIRECT)
	{
		printf("DEBUG: Detected output redirection.\n");
		*current_token = (*current_token)->next;
		if (*current_token && (*current_token)->type == TOKEN_FILENAME)
		{
			redirect = strdup((*current_token)->value);
			if (!redirect)
			{
				fprintf(stderr, "Error: Failed to allocate memory.\n");
				return (-1);
			}
			add_memory(memories, redirect);
			current_command->output_redirect = redirect;
			current_command->append_mode = 0;
		}
		else
		{
			fprintf(stderr, "Error: Expected filename.\n");
			return (-1);
		}
	}
	else if ((*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT)
	{
		printf("DEBUG: Detected append output redirection.\n");
		*current_token = (*current_token)->next;
		if (*current_token && (*current_token)->type == TOKEN_FILENAME)
		{
			redirect = strdup((*current_token)->value);
			if (!redirect)
			{
				fprintf(stderr, "Error: Failed to allocate memory.\n");
				return (-1);
			}
			add_memory(memories, redirect);
			printf("DEBUG: Handling append output redirection: %s\n", redirect);
			current_command->output_redirect = redirect;
			current_command->append_mode = 1;
		}
		else
		{
			fprintf(stderr, "Error: Expected filename.\n");
			return (-1);
		}
	}
	else
	{
		fprintf(stderr, "Unexpected token: %d\n", (*current_token)->type);
		return (-1);
	}
	return (0);
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

int	handle_all_redirections(t_token **current_token, t_command *current_command,
				t_memories *memories)
{
	if (!current_token || !*current_token || !current_command || !memories)
	{
		return (-1);
	}
	if ((*current_token)->type == TOKEN_INPUT_REDIRECT)
	{
		if (handle_redirections(current_token, current_command, memories) == -1)
		{
			fprintf(stderr, "Error handling input redirection\n");
			return (-1);
		}
	}
	else if ((*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT)
	{
		if (handle_redirections(current_token, current_command, memories) == -1)
		{
			fprintf(stderr, "Error handling append output redirection\n");
			return (-1);
		}
	}
	else if ((*current_token)->type == TOKEN_OUTPUT_REDIRECT)
	{
		if (handle_redirections(current_token, current_command, memories) == -1)
		{
			fprintf(stderr, "Error handling overwrite output redirection\n");
			return (-1);
		}
	}
	else
	{
		fprintf(stderr, "Error: Unexpected: %d\n", (*current_token)->type);
		return (-1);
	}
	return (0);
}
