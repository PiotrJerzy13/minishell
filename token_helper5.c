/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper5.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 14:11:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 14:20:15 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	tokenize_input(char *input, t_token_context *context)
{
	context->expect_filename = 0;
	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
		if (*input == '$')
		{
			process_variable_expansion(&input, context);
		}
		else if (*input == '"' || *input == '\'')
		{
			process_quoted_token(&input, context);
		}
		else
		{
			process_general_token(&input, context);
		}
	}
}

char	*get_quoted_token(char **input_ptr, t_env *environment)
{
	char	quote_char;

	quote_char = **input_ptr;
	if (quote_char == '\'')
		return (get_single_quoted_token(input_ptr));
	else if (quote_char == '"')
		return (get_double_quoted_token(input_ptr, environment));
	else
		return (NULL);
}
