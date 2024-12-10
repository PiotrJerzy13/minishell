/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:09:48 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 13:00:27 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_general_token(char **input, t_token_context *context)
{
	char	*start;
	char	*token;

	start = *input;
	while (**input && !ft_isspace(**input) && **input != '|' && **input != '<'
		&& **input != '>')
	{
		(*input)++;
	}
	if (*input > start)
	{
		token = ft_strndup(start, *input - start, context->memories);
		if (context->expect_filename)
		{
			add_token(context->token_list, init_token(token, TOKEN_FILENAME,
					context->memories));
		}
		else
			add_token(context->token_list, init_token(token, TOKEN_COMMAND,
					context->memories));
		free(token);
		context->expect_filename = 0;
	}
	handle_special_characters(input, context);
}

int	handle_heredoc_token(t_token **current_token, t_command **current_command,
		t_memories *memories)
{
	if (!*current_command)
	{
		printf("Error: Heredoc token without command.\n");
		return (-1);
	}
	if (handle_heredoc(current_token, *current_command, memories) == -1)
	{
		printf("Error: Heredoc token without command.\n");
		return (-1);
	}
	return (0);
}

int	handle_redirection_token(t_token **current_token,
	t_command **current_command, t_memories *memories)
{
	if (!*current_command)
	{
		printf("Error: Redirection token without command.\n");
		return (-1);
	}
	if (handle_all_redirections(current_token, *current_command,
			memories) == -1)
	{
		printf("Error: Failed to handle redirection.\n");
		return (-1);
	}
	return (0);
}

int	handle_pipe_token(t_token **current_token, t_command **current_command,
	int *arg_count)
{
	if (!*current_command)
	{
		printf("Error: Pipe '|' without preceding command.\n");
		return (-1);
	}
	handle_pipe(current_command, arg_count);
	if (!(*current_token)->next || (*current_token)->next->type
		!= TOKEN_COMMAND)
	{
		printf("Error: Pipe '|' without following command.\n");
		return (-1);
	}
	return (0);
}

int	process_special_tokens(t_token **current_token, t_command **current_command,
	t_memories *memories, int *arg_count)
{
	if ((*current_token)->type == TOKEN_HEREDOC)
		return (handle_heredoc_token(current_token, current_command, memories));
	if ((*current_token)->type == TOKEN_OUTPUT_REDIRECT
		|| (*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT
		|| (*current_token)->type == TOKEN_INPUT_REDIRECT)
		return (handle_redirection_token(current_token, current_command,
				memories));
	if ((*current_token)->type == TOKEN_PIPE)
		return (handle_pipe_token(current_token, current_command, arg_count));
	printf("Unrecognized special token: %s\n",
		(*current_token)->value);
	return (-1);
}
