/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_helper2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 16:41:11 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 16:58:51 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

void	handle_heredoc(t_token **current_token, t_command *current_command)
{
	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		current_command->heredoc_list = NULL;
		if (collect_heredoc_input((*current_token)->value,
				&current_command->heredoc_list) == -1)
		{
			fprintf(stderr, "Error collecting heredoc input\n");
		}
	}
}

char	*strdup_and_track(const char *value, t_memories *memories)
{
	char	*copy;

	copy = strdup(value);
	if (!copy)
	{
		fprintf(stderr, "Error: Memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, copy);
	return (copy);
}

void	handle_redirections(t_token **current_token, t_command *current_command,
		t_memories *memories, int append_mode)
{
	char	*redirect;

	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		redirect = strdup((*current_token)->value);
		add_memory(memories, redirect);
		if (append_mode)
		{
			current_command->output_redirect = redirect;
			current_command->append_mode = 1;
		}
		else if ((*current_token - 1)->type == TOKEN_OUTPUT_REDIRECT)
		{
			current_command->output_redirect = redirect;
		}
		else if ((*current_token - 1)->type == TOKEN_INPUT_REDIRECT)
		{
			current_command->input_redirect = redirect;
		}
	}
}

void	process_special_tokens(t_token **current_token,
	t_command **current_command, t_memories *memories, int *arg_count)
{
	if ((*current_token)->type == TOKEN_HEREDOC && *current_command)
	{
		handle_heredoc(current_token, *current_command);
	}
	else if (((*current_token)->type == TOKEN_OUTPUT_REDIRECT
			|| (*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT
			|| (*current_token)->type == TOKEN_INPUT_REDIRECT)
		&& *current_command)
	{
		handle_all_redirections(current_token, *current_command, memories);
	}
	else if ((*current_token)->type == TOKEN_PIPE && *current_command)
	{
		handle_pipe(current_command, arg_count);
	}
}
