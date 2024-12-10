/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 12:36:26 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 21:17:17 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	validate_quotes(const char *input)
{
	int	single_quote;
	int	double_quote;

	single_quote = 0;
	double_quote = 0;
	while (*input)
	{
		if (*input == '\'' && double_quote == 0)
			single_quote ^= 1;
		else if (*input == '"' && single_quote == 0)
			double_quote ^= 1;
		input++;
	}
	if (single_quote || double_quote)
	{
		if (single_quote)
			printf("minishell: syntax error:\n");
		if (double_quote)
			printf("minishell: syntax error:\n");
		return (0);
	}
	return (1);
}

char	*get_user_input(void)
{
	char	*input;

	input = readline("minishell> ");
	if (!input)
	{
		return (NULL);
	}
	if (*input == '\0')
	{
		free(input);
		return ("");
	}
	if (!validate_quotes(input))
	{
		printf("minishell: syntax error: unterminated quote\n");
		free(input);
		return ("");
	}
	return (input);
}

int	process_token(t_parse_context *context)
{
	t_token	*token;

	token = *(context->token_ptr);
	if (!(*(context->command)) && token->type == TOKEN_COMMAND)
	{
		*(context->command) = initialize_command(token, context->command_list,
				context->memories);
		if (!(*(context->command)))
			return (-1);
		*(context->arg_count) = 1;
	}
	else if ((token->type == TOKEN_ARGUMENT || token->type == TOKEN_COMMAND)
		&& *(context->command))
	{
		add_argument_to_command(token, *(context->command), context->memories,
			context->arg_count);
	}
	else
	{
		if (process_special_tokens(context->token_ptr, context->command,
				context->memories, context->arg_count) == -1)
			return (-1);
	}
	return (0);
}

int	parse_input_to_commands(t_token *token_list, t_command **command_list,
	t_memories *memories)
{
	t_command		*command;
	t_token			*token;
	int				arg_count;
	t_parse_context	context;

	command = NULL;
	token = token_list;
	arg_count = 0;
	if (!token_list)
		return (0);
	context.token_ptr = &token;
	context.command = &command;
	context.command_list = command_list;
	context.memories = memories;
	context.arg_count = &arg_count;
	while (token)
	{
		if (process_token(&context) == -1)
			return (-1);
		token = token->next;
	}
	if (command)
		command->args[arg_count] = NULL;
	return (0);
}

t_command	*create_new_command(t_memories *memories)
{
	t_command	*command;

	command = malloc(sizeof(t_command));
	if (!command)
	{
		printf("Error: Failed to allocate memory for command.\n");
		return (NULL);
	}
	add_memory(memories, command);
	command->command = NULL;
	command->args = NULL;
	command->input_redirect = NULL;
	command->output_redirect = NULL;
	command->append_mode = 0;
	command->next = NULL;
	return (command);
}
