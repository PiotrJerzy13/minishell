/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 22:08:02 by pwojnaro         ###   ########.fr       */
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
	new_token->value = ft_strndup(value, ft_strlen(value), memories);
	if (!new_token->value)
	{
		printf("Error: Failed to allocate memory for token value.\n");
		exit(EXIT_FAILURE);
	}
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

void	tokenize_input(char *input, t_token_context *context)
{
	context->expect_filename = 0;
	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
		if (*input == '$' && *(input + 1) == '\0')
			printf("$");
		if (*input == '$' && *(input + 1) == '$')
		{
			handle_echo_with_pid(input);
		}
		if (*input == '$')
		{
			process_variable_expansion(&input, context);
		}
		else if (*input == '"' || *input == '\'')
		{
			process_quoted_token(&input, context);
		}
		else
		{
			process_general_token(&input, context);
		}
	}
}
