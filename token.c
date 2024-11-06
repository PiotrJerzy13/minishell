/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/06 20:18:20 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// init_token function allocates memory for a new token and initializes its val
// and type fields. The value is copied using strdup to ensure that the token
// has its own copy of the string. The token is added to the memory tracking
// system using the add_memory function.
// add_token function is used to add a new token to the end of a linked list of
// tokens. If the list is empty, the new token is set as the head of the list.
// tokenize_input function is used to split the input string into tokens. It
// iterates over the input string, skipping leading spaces, and calls
// handle_special_characters when it encounters special characters like pipes or
// redirections. If the character is a quote, it calls get_quoted_token to parse
// the quoted string. Otherwise, it reads the token until it encounters a space
// or special character. The token is then added to the token list.
// handle_special_characters function is used to handle special characters like
// pipes, input redirections, and output redirections. It adds the corresponding
// token to the token list and advances the input pointer to the next character.
// Handle_special is called by tokenize_input not working for > and >>

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
	char	*result;
	char	*var_name;
	char	*value;

	result = malloc(1);
	result[0] = '\0';
	quote_char = **input_ptr;
	start = *input_ptr + 1;
	end = start;
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
				result = realloc(result, strlen(result) + strlen(value) + 1);
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
	if (*end == quote_char)
		*input_ptr = end + 1;
	else
		*input_ptr = end;
	return (result);
}

void	handle_special_characters(char **input, t_token **token_list,
		t_memories *memories, int *expect_filename)
{
	if (**input == '|')
	{
		add_token(token_list, init_token("|", TOKEN_PIPE, memories));
		printf("Tokenized pipe: %s\n", "|");
		(*input)++;
	}
	else if (**input == '<')
	{
		add_token(token_list, init_token("<", TOKEN_INPUT_REDIRECT, memories));
		printf("Tokenized input redirection: %s\n", "<");
		(*input)++;
		*expect_filename = 1;
	}
	else if (**input == '>')
	{
		if (*(*input + 1) == '>')
		{
			add_token(token_list, init_token(">>", TOKEN_APPEND_OUTPUT_REDIRECT,
					memories));
			printf("Tokenized append output redirection: %s\n", ">>");
			(*input) += 2;
		}
		else
		{
			add_token(token_list, init_token(">", TOKEN_OUTPUT_REDIRECT,
					memories));
			printf("Tokenized output redirection: %s\n", ">");
			(*input)++;
		}
		*expect_filename = 1;
	}
}

void	tokenize_input(char *input, t_token **token_list, t_memories *memories,
	t_env *environment)
{
	char			*token;
	char			*start;
	int				expect_filename;
	t_token_type	type;
	char			*value;

	expect_filename = 0;
	printf("Starting tokenization process...\n");
	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
		if (*input == '$')
		{
			input++;
			start = input;
			while (isalnum(*input) || *input == '_')
				input++;
			token = strndup(start, input - start);
			value = get_env_value(token, environment);
			if (value != NULL)
			{
				add_token(token_list, init_token(value,
						TOKEN_ARGUMENT, memories));
				free(value);
			}
			else
			{
				add_token(token_list, init_token("", TOKEN_ARGUMENT, memories));
			}
			free(token);
		}
		else if (*input == '"' || *input == '\'')
		{
			token = get_quoted_token(&input, environment);
			if (token)
			{
				if (expect_filename)
					type = TOKEN_FILENAME;
				else
					type = TOKEN_ARGUMENT;
				add_token(token_list, init_token(token, type, memories));
				printf("Tokenized quoted argument: %s\n", token);
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
					type = TOKEN_FILENAME;
				else
					type = TOKEN_COMMAND;
				add_token(token_list, init_token(token, type, memories));
				printf("Tokenized command/argument: %s\n", token);
				free(token);
				expect_filename = 0;
			}
			handle_special_characters(&input, token_list,
				memories, &expect_filename);
		}
	}
	printf("Tokenization complete.\n");
}
