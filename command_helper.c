/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 15:24:01 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/17 16:07:45 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**env_to_char_array(t_env *environment)
{
	char	**env_array;
	size_t	len;
	int		i;

	i = 0;
	env_array = malloc((environment->size + 1) * sizeof(char *));
	if (!env_array)
	{
		exit(EXIT_FAILURE);
	}
	while (i < environment->size)
	{
		len = strlen(environment->pairs[i].key)
			+ strlen(environment->pairs[i].value) + 2;
		env_array[i] = malloc(len);
		if (!env_array[i])
		{
			exit(EXIT_FAILURE);
		}
		snprintf(env_array[i], len, "%s=%s", environment->pairs[i].key,
			environment->pairs[i].value);
		i++;
	}
	env_array[environment->size] = NULL;
	return (env_array);
}

void	free_env_array(char **env_array)
{
	int	i;

	i = 0;
	while (env_array[i] != NULL)
	{
		free(env_array[i]);
		i++;
	}
	free(env_array);
}

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
