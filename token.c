/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/24 17:35:43 by pwojnaro         ###   ########.fr       */
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

void	tokenize_input(char *input, t_token_context *context)
{
	context->expect_filename = 0;
	while (*input)
	{
		skip_spaces(&input);
		if (*input == '\0')
			break ;
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

void	handle_token_creation(char **input, t_token_context *context,
	t_token_info *info)
{
	if (*(*input + 1) == info->single_token[0])
	{
		add_token(context->token_list, init_token(info->double_token,
				info->type_double, context->memories));
		(*input) += 2;
	}
	else
	{
		add_token(context->token_list, init_token(info->single_token,
				info->type_single, context->memories));
		(*input)++;
	}
	context->expect_filename = 1;
}
