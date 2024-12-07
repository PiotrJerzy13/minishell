/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: piotr <piotr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:09:48 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/07 20:22:45 by piotr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_general_token(char **input, t_token_context *context)
{
	char	*start;
	char	*token;

	start = *input;
	while (**input && !isspace(**input) && **input != '|' && **input != '<'
		&& **input != '>')
	{
		(*input)++;
	}
	if (*input > start)
	{
		token = strndup(start, *input - start);
		if (context->expect_filename)
		{
			add_token(context->token_list, init_token(token, TOKEN_FILENAME,
					context->memories));
		}
		else
		{
			add_token(context->token_list, init_token(token, TOKEN_COMMAND,
					context->memories));
		}
		free(token);
		context->expect_filename = 0;
	}
	handle_special_characters(input, context);
}

void	process_quoted_token(char **input, t_token_context *context)
{
	char	*token;

	token = NULL;
	if (**input == '"')
	{
		token = get_double_quoted_token(input, context->environment,
				context->memories);
	}
	else if (**input == '\'')
	{
		token = get_single_quoted_token(input);
	}
	if (token)
	{
		if (context->expect_filename)
		{
			add_token(context->token_list, init_token(token, TOKEN_FILENAME,
					context->memories));
		}
		else
		{
			add_token(context->token_list, init_token(token, TOKEN_ARGUMENT,
					context->memories));
		}
		free(token);
	}
}

void	handle_special_characters(char **input, t_token_context *context)
{
	if (**input == '|')
	{
		add_token(context->token_list, init_token("|",
				TOKEN_PIPE, context->memories));
		(*input)++;
	}
	else if (**input == '<')
	{
		if (*(*input + 1) == '<')
		{
			add_token(context->token_list, init_token("<<",
					TOKEN_HEREDOC, context->memories));
			(*input) += 2;
		}
		else
		{
			add_token(context->token_list, init_token("<",
					TOKEN_INPUT_REDIRECT, context->memories));
			(*input)++;
		}
		context->expect_filename = 1;
	}
	else if (**input == '>')
	{
		if (*(*input + 1) == '>')
		{
			add_token(context->token_list, init_token(">>",
					TOKEN_APPEND_OUTPUT_REDIRECT, context->memories));
			(*input) += 2;
		}
		else
		{
			add_token(context->token_list, init_token(">",
					TOKEN_OUTPUT_REDIRECT, context->memories));
			(*input)++;
		}
		context->expect_filename = 1;
	}
	if (context->expect_filename && (**input == '\0' || **input == '|'
			|| **input == '<' || **input == '>'))
	{
		*(context->last_exit_status) = 258;
	}
}

int	process_special_tokens(t_token **current_token, t_command **current_command,
				t_memories *memories, int *arg_count)
{
	if ((*current_token)->type == TOKEN_HEREDOC && *current_command)
	{
		if (handle_heredoc(current_token, *current_command) == -1)
		{
			fprintf(stderr, "Error: Failed to handle heredoc.\n");
			return (-1);
		}
	}
	else if (((*current_token)->type == TOKEN_OUTPUT_REDIRECT
			|| (*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT
			|| (*current_token)->type == TOKEN_INPUT_REDIRECT)
		&& *current_command)
	{
		if (handle_all_redirections(current_token, *current_command,
				memories) == -1)
		{
			fprintf(stderr, "Error: Failed to handle redirection.\n");
			return (-1);
		}
	}
	else if ((*current_token)->type == TOKEN_PIPE)
	{
		if (!*current_command)
		{
			fprintf(stderr, "Error: Pipe '|' without preceding command.\n");
			return (-1);
		}
		handle_pipe(current_command, arg_count);
		if (!(*current_token)->next || (*current_token)->next->type
			!= TOKEN_COMMAND)
		{
			fprintf(stderr, "Error: Pipe '|' without following command.\n");
			return (-1);
		}
	}
	else
	{
		fprintf(stderr, "Unrecognized special token: %s\n",
			(*current_token)->value);
		return (-1);
	}
	return (0);
}
