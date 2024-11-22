/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/19 17:52:59 by pwojnaro         ###   ########.fr       */
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

void	handle_exit_status_expansion(char **input, t_token_context *context)
{
	char	exit_status_str[12];

	snprintf(exit_status_str, sizeof(exit_status_str),
		"%d", *(context->last_exit_status));
	add_token(context->token_list, init_token(exit_status_str,
			TOKEN_ARGUMENT, context->memories));
	*input += 2;
}

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
