/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/27 14:02:37 by pwojnaro         ###   ########.fr       */
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

char	*get_quoted_token(char **input_ptr)
{
	char	*start;
	char	*end;
	char	quote_char;

	quote_char = **input_ptr;
	start = *input_ptr + 1;
	end = start;
	while (*end && *end != quote_char)
	{
		end++;
	}
	if (*end == quote_char)
	{
		*end = '\0';
		*input_ptr = end + 1;
		return (strdup(start));
	}
	printf("Error: Unmatched quote in input.\n");
	return (NULL);
}

void	tokenize_input(char *input, t_token **token_list, t_memories *memories)
{
	char	*token;
	char	*start;

	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
		if (*input == '"' || *input == '\'')
		{
			token = get_quoted_token(&input);
			if (token)
			{
				add_token(token_list, init_token(token, TOKEN_ARGUMENT,
						memories));
				free(token);
			}
		}
		else
		{
			start = input;
			while (*input && !isspace(*input) && *input != '|' && *input != '<'
				&& *input != '>')
				input++;
			if (input > start)
			{
				token = strndup(start, input - start);
				add_token(token_list, init_token(token, TOKEN_COMMAND,
						memories));
				free(token);
			}
			handle_special_characters(&input, token_list, memories);
		}
	}
}

void	handle_special_characters(char **input, t_token **token_list,
	t_memories *memories)
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
	}
	else if (**input == '>')
	{
		if (*(*input + 1) == '>')
		{
			add_token(token_list, init_token(">>", TOKEN_APPEND_OUTPUT_REDIRECT,
					memories));
			(*input) += 2;
		}
		else
		{
			add_token(token_list, init_token(">", TOKEN_OUTPUT_REDIRECT,
					memories));
			(*input)++;
		}
	}
}
