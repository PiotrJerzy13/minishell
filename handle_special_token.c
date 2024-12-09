/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_special_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 13:40:51 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 13:54:13 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
			add_token(context->token_list, init_token(token, TOKEN_ARGUMENT,
					context->memories));
		free(token);
	}
}

// void	handle_special_characters(char **input, t_token_context *context)
// {
// 	if (**input == '|')
// 	{
// 		add_token(context->token_list, init_token("|",
// 				TOKEN_PIPE, context->memories));
// 		(*input)++;
// 	}
// 	else if (**input == '<')
// 	{
// 		if (*(*input + 1) == '<')
// 		{
// 			add_token(context->token_list, init_token("<<",
// 					TOKEN_HEREDOC, context->memories));
// 			(*input) += 2;
// 		}
// 		else
// 		{
// 			add_token(context->token_list, init_token("<",
// 					TOKEN_INPUT_REDIRECT, context->memories));
// 			(*input)++;
// 		}
// 		context->expect_filename = 1;
// 	}
// 	else if (**input == '>')
// 	{
// 		if (*(*input + 1) == '>')
// 		{
// 			add_token(context->token_list, init_token(">>",
// 					TOKEN_APPEND_OUTPUT_REDIRECT, context->memories));
// 			(*input) += 2;
// 		}
// 		else
// 		{
// 			add_token(context->token_list, init_token(">",
// 					TOKEN_OUTPUT_REDIRECT, context->memories));
// 			(*input)++;
// 		}
// 		context->expect_filename = 1;
// 	}
// 	if (context->expect_filename && (**input == '\0' || **input == '|'
// 			|| **input == '<' || **input == '>'))
// 	{
// 		*(context->last_exit_status) = 258;
// 	}
// }
