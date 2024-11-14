/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: piotr <piotr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/14 20:38:05 by piotr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*init_token(char *value, t_token_type type, t_memories *memories)
{
	t_token	*new_token;

	new_token = (t_token *)malloc(sizeof(t_token));
	if (!new_token)
	{
		printf("Error: Failed to allocate memory for token.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, new_token);
	new_token->value = strdup(value);
	add_memory(memories, new_token->value);
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
	}
	else
	{
		current = *head;
		while (current->next)
		{
			current = current->next;
		}
		current->next = new_token;
	}
}

char	*get_quoted_token(char **input_ptr, t_env *environment)
{
	char	*start;
	char	*end;
	char	quote_char;
	char	*var_name;
	char	*value;
	char	*result = malloc(1);

	result[0] = '\0';
	quote_char = **input_ptr;
	start = *input_ptr + 1;
	end = start;
	if (quote_char == '\'')
	{
		while (*end && *end != quote_char)
			end++;
		strncat(result, start, end - start);
		*input_ptr = (*end == quote_char) ? end + 1 : end;
	}
	else if (quote_char == '"')
	{
		while (*end && *end != quote_char)
		{
			if (*end == '$')
			{
				strncat(result, start, end - start);
				end++;
				start = end;
				while (isalnum(*end) || *end == '_')
					end++;
				var_name = strndup(start, end - start);
				value = get_env_value(var_name, environment);
				free(var_name);
				if (value)
				{
					result = realloc(result, strlen(result)
							+ strlen(value) + 1);
					strcat(result, value);
					free(value);
				}
				start = end;
			}
			else
			{
				end++;
			}
		}
		strncat(result, start, end - start);
		*input_ptr = (*end == quote_char) ? end + 1 : end;
	}
	return (result);
}

void	handle_special_characters(char **input, t_token **token_list,
			t_memories *memories, int *expect_filename, int *last_exit_status)
{
	if (**input == '|')
	{
		add_token(token_list, init_token("|", TOKEN_PIPE, memories));
		(*input)++;
	}
	else if (**input == '<')
	{
		add_token(token_list, init_token("<", TOKEN_INPUT_REDIRECT, memories));
		(*input)++;
		*expect_filename = 1;
	}
	else if (**input == '>')
	{
		if (*(*input + 1) == '>')
		{
			add_token(token_list, init_token(">>",
					TOKEN_APPEND_OUTPUT_REDIRECT, memories));
			(*input) += 2;
		}
		else
		{
			add_token(token_list, init_token(">",
					TOKEN_OUTPUT_REDIRECT, memories));
			(*input)++;
		}
		*expect_filename = 1;
	}
	if (*expect_filename && (**input == '\0' || **input == '|'
			|| **input == '<' || **input == '>'))
	{
		*last_exit_status = 258;
		return ;
	}
}

void	tokenize_input(char *input, t_token **token_list, t_memories *memories,
			t_env *environment, int *last_exit_status)
{
	char	*token;
	char	*start;
	int		expect_filename;
	char	*value;
	char	exit_status_str[12];

	expect_filename = 0;
	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
		if (*input == '$')
		{
			if (*(input + 1) == '?')
			{
				snprintf(exit_status_str, sizeof(exit_status_str),
					"%d", *last_exit_status);
				add_token(token_list, init_token(exit_status_str,
						TOKEN_ARGUMENT, memories));
				input += 2;
				continue ;
			}
			else
			{
				input++;
				start = input;
				while (isalnum(*input) || *input == '_')
					input++;
				token = strndup(start, input - start);
				value = get_env_value(token, environment);
				if (value)
					add_token(token_list, init_token(value,
							TOKEN_ARGUMENT, memories));
				else
					add_token(token_list, init_token("",
							TOKEN_ARGUMENT, memories));
				free(token);
				if (value)
					free(value);
			}
		}
		else if (*input == '"' || *input == '\'')
		{
			token = get_quoted_token(&input, environment);
			if (token)
			{
				if (expect_filename)
					add_token(token_list, init_token(token,
							TOKEN_FILENAME, memories));
				else
					add_token(token_list, init_token(token,
							TOKEN_ARGUMENT, memories));
				free(token);
				expect_filename = 0;
			}
		}
		else
		{
			start = input;
			while (*input && !isspace(*input) && *input != '|'
				&& *input != '<' && *input != '>')
				input++;
			if (input > start)
			{
				token = strndup(start, input - start);
				if (expect_filename)
					add_token(token_list, init_token(token,
							TOKEN_FILENAME, memories));
				else
					add_token(token_list, init_token(token,
							TOKEN_COMMAND, memories));
				free(token);
				expect_filename = 0;
			}
			handle_special_characters(&input, token_list, memories,
				&expect_filename, last_exit_status);
		}
	}
}
