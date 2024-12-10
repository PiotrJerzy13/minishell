/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_special_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 13:40:51 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 12:24:22 by pwojnaro         ###   ########.fr       */
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
		token = get_single_quoted_token(input, context->memories);
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
