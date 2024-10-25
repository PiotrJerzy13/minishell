/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/23 12:47:01 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*init_command_node(void)
{
	t_command	*new_node;

	new_node = (t_command *)malloc(sizeof(t_command));
	if (!new_node)
	{
		printf("Error: Failed to allocate memory for command node.\n");
		exit(EXIT_FAILURE);
	}
	new_node->command = NULL;
	new_node->args = NULL;
	new_node->is_pipe = 0;
	new_node->input_redirect = NULL;
	new_node->output_redirect = NULL;
	new_node->next = NULL;
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

void	free_command_list(t_command *head)
{
	t_command	*tmp;
	int			i;

	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp->command);
		if (tmp->args)
		{
			i = 0;
			while (tmp->args[i])
				free(tmp->args[i++]);
			free(tmp->args);
		}
		free(tmp->input_redirect);
		free(tmp->output_redirect);
		free(tmp);
	}
}

void	parse_input_to_commands(t_command **command_list, t_memories *memories)
{
	char		*input;
	t_command	*new_command;
	char		*token;

	input = allocate_user_input(memories);
	token = strtok(input, " ");
	while (token)
	{
		new_command = init_command_node();
		new_command->command = strdup(token);
		add_memory(memories, new_command->command);
		add_command_node(command_list, new_command);
		token = strtok(NULL, " ");
	}
}
