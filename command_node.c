/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_node.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 15:24:01 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/04 12:17:59 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*initialize_command(t_token *token,
	t_command **command_list, t_memories *memories)
{
	t_command	*new_command;
	t_command	*current;

	new_command = create_new_command(memories);
	if (!new_command)
	{
		printf("Error: Failed to create a new command.\n");
		return (NULL);
	}
	new_command->command = strdup(token->value);
	add_memory(memories, new_command->command);
	new_command->args = malloc(sizeof(char *) * (MAX_ARGS));
	if (!new_command->args)
	{
		printf("Error: Failed to allocate memory for arguments.\n");
		return (NULL);
	}
	add_memory(memories, new_command->args);
	new_command->args[0] = strdup(token->value);
	add_memory(memories, new_command->args[0]);
	if (*command_list == NULL)
	{
		*command_list = new_command;
	}
	else
	{
		current = *command_list;
		while (current->next)
		{
			current = current->next;
		}
		current->next = new_command;
	}
	return (new_command);
}
