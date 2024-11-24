/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 12:36:26 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/24 14:35:10 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
This code is needed for processing and dynamically building a string 
result in scenarios where parts of an 
input string need to be parsed, expanded, and combined into a final 
string. Example echo "Hello, $USER!"
**/
void	append_to_result(char **result, const char *start, size_t length)
{
	*result = realloc(*result, strlen(*result) + length + 1);
	if (!*result)
		exit(EXIT_FAILURE);
	strncat(*result, start, length);
}

void	handle_expansion(char **result, char **end_ptr, t_env *environment)
{
	char	*end;
	char	*start;
	char	*var_name;
	char	*value;

	end = *end_ptr;
	start = end;
	while (isalnum(*end) || *end == '_')
		end++;
	var_name = strndup(start, end - start);
	if (!var_name)
		exit(EXIT_FAILURE);
	value = get_env_value(var_name, environment);
	free(var_name);
	if (value)
	{
		append_to_result(result, value, strlen(value));
		free(value);
	}
	*end_ptr = end;
}

void	process_until_special(char **end_ptr, char **start_ptr, char **result,
	char stop_char)
{
	char	*end;
	char	*start;

	end = *end_ptr;
	start = *start_ptr;
	while (*end && *end != stop_char && *end != '$')
		end++;
	append_to_result(result, start, end - start);
	*end_ptr = end;
	*start_ptr = end;
}

void	handle_dollar(char **end, char **start,
	char **result, t_env *environment)
{
	process_until_special(end, start, result, '$');
	handle_expansion(result, end, environment);
	*start = *end;
}

char	*get_user_input(void)
{
	char	*input;
	char	*line;

	input = NULL;
	line = NULL;
	if (isatty(fileno(stdin)))
		input = readline("minishell> ");
	else
	{
		line = get_next_line(fileno(stdin));
		if (!line)
			return (NULL);
		input = ft_strtrim(line, "\n");
		free(line);
	}
	return (input);
}

void	parse_input_to_commands(t_token *token_list, t_command **command_list, t_memories *memories)
{
	t_command	*command;
	t_token		*token;
	int			arg_count;

	command = NULL;
	token = token_list;
	arg_count = 0;
	while (token)
	{
		printf("Parsing token: %s (type: %d)\n", token->value, token->type);
		if (command == NULL && token->type == TOKEN_COMMAND)
		{
			printf("Initializing new command: %s\n", token->value);
			command = initialize_command(token, command_list, memories);
			if (!command)
			{
				printf("Error: Failed to initialize command.\n");
				return ;
			}
			arg_count = 1;
		}
		else if ((token->type == TOKEN_ARGUMENT || token->type == TOKEN_COMMAND)
			&& command)
		{
			printf("Adding argument: %s to command: %s\n", token->value,
				command->command);
			add_argument_to_command(token, command, memories, &arg_count);
		}
		else
		{
			printf("Processing special token: %s (type: %d)\n",
				token->value, token->type);
			process_special_tokens(&token, &command, memories, &arg_count);
		}
		token = token->next;
	}
	if (command)
	{
		command->args[arg_count] = NULL;
		printf("Finalized command: %s with %d arguments.\n",
			command->command, arg_count);
		for (int i = 0; i < arg_count; i++)
		{
			printf("  Argument[%d]: %s\n", i, command->args[i]);
		}
	}
}

t_command *create_new_command(t_memories *memories)
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
