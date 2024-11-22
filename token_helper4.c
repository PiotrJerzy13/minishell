/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:09:48 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 14:19:51 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_general_token(char **input, t_token_context *context)
{
	char	*start;
	char	*token;

	start = *input;
	while (**input && !isspace(**input) && **input != '|'
		&& **input != '<' && **input != '>')
		(*input)++;
	if (*input > start)
	{
		token = strndup(start, *input - start);
		if (context->expect_filename)
		{
			add_token(context->token_list, init_token(token,
					TOKEN_FILENAME, context->memories));
		}
		else
		{
			add_token(context->token_list, init_token(token,
					TOKEN_COMMAND, context->memories));
		}
		free(token);
		context->expect_filename = 0;
	}
	handle_special_characters(input, context);
}

void	handle_token_creation(char **input, t_token_context *context,
			t_token_info *info)
{
	if (*(*input + 1) == info->single_token[0])
	{
		add_token(context->token_list, init_token(info->double_token,
				info->type_double, context->memories));
		(*input) += 2;
	}
	else
	{
		add_token(context->token_list, init_token(info->single_token,
				info->type_single, context->memories));
		(*input)++;
	}
	context->expect_filename = 1;
}

void	handle_redirects(char **input, t_token_context *context,
			char redirect_char)
{
	t_token_info	info;

	if (redirect_char == '<')
	{
		info.single_token = "<";
		info.double_token = "<<";
		info.type_single = TOKEN_INPUT_REDIRECT;
		info.type_double = TOKEN_HEREDOC;
	}
	else
	{
		info.single_token = ">";
		info.double_token = ">>";
		info.type_single = TOKEN_OUTPUT_REDIRECT;
		info.type_double = TOKEN_APPEND_OUTPUT_REDIRECT;
	}
	handle_token_creation(input, context, &info);
}

void	handle_special_characters(char **input, t_token_context *context)
{
	if (**input == '|')
	{
		add_token(context->token_list, init_token("|",
				TOKEN_PIPE, context->memories));
		(*input)++;
	}
	else if (**input == '<' || **input == '>')
		handle_redirects(input, context, **input);
	if (context->expect_filename && (**input == '\0' || **input == '|'
			|| **input == '<' || **input == '>'))
		*(context->last_exit_status) = 258;
}

void	process_quoted_token(char **input, t_token_context *context)
{
	char	*token;

	token = get_quoted_token(input, context->environment);
	if (token)
	{
		if (context->expect_filename)
		{
			add_token(context->token_list, init_token(token,
					TOKEN_FILENAME, context->memories));
		}
		else
		{
			add_token(context->token_list, init_token(token, TOKEN_ARGUMENT,
					context->memories));
		}
		free(token);
		context->expect_filename = 0;
	}
}
