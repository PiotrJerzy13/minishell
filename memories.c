/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memories.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:49:26 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 21:27:30 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_memories(t_memories *memories, t_env *environment,
	int env_capacity)
{
	size_t	i;

	i = 0;
	memories->capacity = 10;
	memories->size = 0;
	memories->allocations = (void **)malloc(memories->capacity
			* sizeof(void *));
	if (!memories->allocations)
	{
		printf("Error: Failed to initialize memory tracking system.\n");
		exit(EXIT_FAILURE);
	}
	while (i < memories->capacity)
	{
		memories->allocations[i] = NULL;
		i++;
	}
	environment->pairs = (t_key_value *)malloc(env_capacity
			* sizeof(t_key_value));
	if (!environment->pairs)
		exit(EXIT_FAILURE);
	environment->size = 0;
	environment->capacity = env_capacity;
	add_memory(memories, environment->pairs);
}

int	is_in_memories(t_memories *memories, void *ptr)
{
	size_t	i;

	i = 0;
	while (i < memories->size)
	{
		if (memories->allocations[i] == ptr)
			return (1);
		i++;
	}
	return (0);
}

void	add_memory(t_memories *memories, void *ptr)
{
	size_t	new_capacity;
	void	**new_allocations;
	size_t	i;

	i = 0;
	if (!ptr || is_in_memories(memories, ptr))
		return ;
	if (memories->size >= memories->capacity)
	{
		new_capacity = memories->capacity * 2;
		new_allocations = malloc(new_capacity * sizeof(void *));
		if (!new_allocations)
			exit(EXIT_FAILURE);
		while (i < memories->size)
		{
			new_allocations[i] = memories->allocations[i];
			i++;
		}
		free(memories->allocations);
		memories->allocations = new_allocations;
		memories->capacity = new_capacity;
	}
	memories->allocations[memories->size++] = ptr;
}

void	free_all_memories(t_memories *memories)
{
	size_t	i;

	i = 0;
	if (!memories || !memories->allocations)
	{
		printf("Warning: Attempted to free an uninitialized memory.\n");
		return ;
	}
	while (i < memories->size)
	{
		if (memories->allocations[i])
		{
			free(memories->allocations[i]);
			memories->allocations[i] = NULL;
		}
		i++;
	}
	free(memories->allocations);
	memories->allocations = NULL;
	memories->size = 0;
	memories->capacity = 0;
}

void	remove_memory(t_memories *memories, void *ptr)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (i < memories->size)
	{
		if (memories->allocations[i] == ptr)
		{
			j = i;
			while (j < memories->size - 1)
			{
				memories->allocations[j] = memories->allocations[j + 1];
				j++;
			}
			memories->size--;
			return ;
		}
		i++;
	}
}
