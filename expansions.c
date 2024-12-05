/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansions.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 12:07:21 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/04 13:24:41 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Handle expanding environment variables and special constructs like $?
// (representing the exit status of the last executed command).
void	handle_exit_status_expansion(char **input, t_token_context *context)
{
	char	exit_status_str[12];

	snprintf(exit_status_str, sizeof(exit_status_str),
		"%d", *(context->last_exit_status));
	add_token(context->token_list, init_token(exit_status_str,
			TOKEN_ARGUMENT, context->memories));
	*input += 2;
}

// This function decides how to handle variable expansion based on the 
// character following the $ symbol.
void	process_variable_expansion(char **input, t_token_context *context)
{
	if (*(*input + 1) == '?')
	{
		handle_exit_status_expansion(input, context);
	}
	else
	{
		handle_variable_expansion(input, context);
	}
}

// This function processes standard variable expansions like $HOME or $PATH.
void	handle_variable_expansion(char **input, t_token_context *context)
{
	char	*start;
	char	*token;
	char	*value;

	(*input)++;
	start = *input;
	while (isalnum(**input) || **input == '_')
		(*input)++;
	token = strndup(start, *input - start);
	value = get_env_value(token, context->environment);
	if (value != NULL)
	{
		add_token(context->token_list, init_token(value,
				TOKEN_ARGUMENT, context->memories));
	}
	else
	{
		add_token(context->token_list, init_token("",
				TOKEN_ARGUMENT, context->memories));
	}
	free(token);
	if (value != NULL)
	{
		free(value);
	}
}
