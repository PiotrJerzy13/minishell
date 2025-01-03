/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_node.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 15:24:01 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 23:06:41 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*create_command_node(t_token *token, t_memories *memories)
{
	t_command	*new_command;

	new_command = create_new_command(memories);
	if (!new_command)
		return (NULL);
	new_command->command = ft_strndup(token->value,
			ft_strlen(token->value), memories);
	if (!new_command->command)
		return (NULL);
	new_command->args = malloc(sizeof(char *) * MAX_ARGS);
	if (!new_command->args)
		return (NULL);
	add_memory(memories, new_command->args);
	new_command->args[0] = ft_strndup(token->value,
			ft_strlen(token->value), memories);
	if (!new_command->args[0])
		return (NULL);
	return (new_command);
}

void	add_command_to_list(t_command **command_list, t_command *new_command)
{
	t_command	*current;

	if (*command_list == NULL)
	{
		*command_list = new_command;
	}
	else
	{
		current = *command_list;
		while (current->next)
			current = current->next;
		current->next = new_command;
	}
}

t_command	*initialize_command(t_token *token, t_command **command_list,
	t_memories *memories)
{
	t_command	*new_command;

	new_command = create_command_node(token, memories);
	if (!new_command)
		return (NULL);
	add_command_to_list(command_list, new_command);
	return (new_command);
}

void	restore_redirections(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1)
			perror("Failed to restore stdin");
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		if (dup2(saved_stdout, STDOUT_FILENO) == -1)
			perror("Failed to restore stdout");
		close(saved_stdout);
	}
}
