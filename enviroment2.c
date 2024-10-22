/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:41:14 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/14 15:50:22 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_env_list(t_env_node *head)
{
	t_env_node	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp->key);
		free(tmp->value);
		free(tmp);
	}
}

void	export_env_var(t_env_node **env_list, char *input)
{
	char	*key;
	char	*value;

	key = strtok(input, "=");
	value = strtok(NULL, "=");
	if (key && value)
	{
		add_or_update_env_var(env_list, key, value);
	}
	else
	{
		printf("Error: Invalid format. Use export KEY=VALUE.\n");
	}
}

void	print_env(t_env_node *env_list)
{
	t_env_node	*current;

	current = env_list;
	while (current)
	{
		printf("%s=%s\n", current->key, current->value);
		current = current->next;
	}
}
