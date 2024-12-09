/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_special_char.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 13:53:28 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 14:16:20 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_pipe_character(char **input, t_token_context *context)
{
	add_token(context->token_list, init_token("|", TOKEN_PIPE,
			context->memories));
	(*input)++;
}

void	handle_input_redirection_character(char **input,
	t_token_context *context)
{
	if (*(*input + 1) == '<')
	{
		add_token(context->token_list, init_token("<<", TOKEN_HEREDOC,
				context->memories));
		(*input) += 2;
	}
	else
	{
		add_token(context->token_list, init_token("<", TOKEN_INPUT_REDIRECT,
				context->memories));
		(*input)++;
	}
	context->expect_filename = 1;
}

void	handle_output_redirection_character(char **input,
	t_token_context *context)
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

void	handle_special_characters(char **input, t_token_context *context)
{
	if (**input == '|')
	{
		handle_pipe_character(input, context);
	}
	else if (**input == '<')
	{
		handle_input_redirection_character(input, context);
	}
	else if (**input == '>')
	{
		handle_output_redirection_character(input, context);
	}
	if (context->expect_filename && (**input == '\0' || **input == '|'
			|| **input == '<' || **input == '>'))
	{
		*(context->last_exit_status) = 258;
	}
}
