/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 12:36:26 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/04 12:18:11 by pwojnaro         ###   ########.fr       */
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
			fprintf(stderr, "minishell: syntax error:\n");
		if (double_quote)
			fprintf(stderr, "minishell: syntax error:\n");
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
		printf("DEBUG: EOF detected in get_user_input.\n");
		return (NULL);
	}
	if (*input == '\0')
	{
		free(input);
		return ("");
	}
	if (!validate_quotes(input))
	{
		fprintf(stderr, "minishell: syntax error: unterminated quote\n");
		free(input);
		return ("");
	}
	return (input);
}

int	parse_input_to_commands(t_token *token_list, t_command **command_list,
	t_memories *memories)
{
	t_command	*command;
	t_token		*token;
	int			arg_count;

	command = NULL;
	token = token_list;
	arg_count = 0;
	if (!token_list)
		return (0);
	while (token)
	{
		if (!command && token->type == TOKEN_COMMAND)
		{
			command = initialize_command(token, command_list, memories);
			if (!command)
			{
				fprintf(stderr, "Error: Failed to initialize command.\n");
				return (-1);
			}
			arg_count = 1;
		}
		else if ((token->type == TOKEN_ARGUMENT
				|| token->type == TOKEN_COMMAND) && command)
		{
			add_argument_to_command(token, command, memories, &arg_count);
		}
		else
		{
			if (process_special_tokens(&token, &command,
					memories, &arg_count) == -1)
			{
				fprintf(stderr, "Error: Failed to process special token.\n");
				return (-1);
			}
		}
		token = token->next;
	}
	if (command)
	{
		command->args[arg_count] = NULL;
	}
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
