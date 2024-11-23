/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_node.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 15:24:01 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:37:44 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*init_command_node(t_memories *memories)
{
	t_command	*new_node;

	new_node = (t_command *)malloc(sizeof(t_command));
	if (!new_node)
	{
		printf("Error: Failed to allocate memory for command node.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, new_node);
	new_node->command = NULL;
	new_node->args = NULL;
	new_node->is_pipe = 0;
	new_node->input_redirect = NULL;
	new_node->output_redirect = NULL;
	new_node->next = NULL;
	new_node->heredoc_list = NULL;
	return (new_node);
}

void	add_command_node(t_command **head, t_command *new_command)
{
	t_command	*current;

	current = *head;
	if (!current)
	{
		*head = new_command;
		return ;
	}
	while (current->next)
		current = current->next;
	current->next = new_command;
}

t_command	*initialize_command(t_token *current_token,
		t_command **command_list, t_memories *memories)
{
	t_command	*command;

	command = init_command_node(memories);
	add_command_node(command_list, command);
	command->args = malloc(sizeof(char *) * 10);
	add_memory(memories, command->args);
	command->command = strdup(current_token->value);
	add_memory(memories, command->command);
	command->args[0] = strdup(current_token->value);
	add_memory(memories, command->args[0]);
	return (command);
}
