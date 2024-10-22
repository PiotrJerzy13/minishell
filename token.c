/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/13 15:21:09 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*init_token(char *value, t_token_type type)
{
	t_token	*new_token;

	new_token = (t_token *)malloc(sizeof(t_token));
	if (!new_token)
	{
		printf("Error: Failed to allocate memory for token.\n");
		exit(EXIT_FAILURE);
	}
	new_token->value = strdup(value);
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

	start = *input_ptr;
	end = start + 1;
	while (*end && *end != '"')
	{
		end++;
	}
	if (*end == '"')
	{
		*end = '\0';
		*input_ptr = end + 1;
		return (strdup(start + 1));
	}
	return (NULL);
}

void	tokenize_input(char *input, t_token **token_list)
{
	char	*token;
	char	*start;

	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
		if (*input == '"')
		{
			token = handle_quoted_string(&input);
			if (token)
			{
				add_token(token_list, init_token(token, TOKEN_ARGUMENT));
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
				add_token(token_list, init_token(token, TOKEN_COMMAND));
				free(token);
			}
			handle_special_characters(&input, token_list);
		}
	}
}

void	handle_special_characters(char **input, t_token **token_list)
{
	if (**input == '|')
	{
		add_token(token_list, init_token("|", TOKEN_PIPE));
		(*input)++;
	}
	else if (**input == '<')
	{
		add_token(token_list, init_token("<", TOKEN_INPUT_REDIRECT));
		(*input)++;
	}
	else if (**input == '>')
	{
		if (*(*input + 1) == '>')
		{
			add_token(token_list, init_token(">>",
					TOKEN_APPEND_OUTPUT_REDIRECT));
			(*input) += 2;
		}
		else
		{
			add_token(token_list, init_token(">", TOKEN_OUTPUT_REDIRECT));
			(*input)++;
		}
	}
}
