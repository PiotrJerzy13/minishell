/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memories.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:49:26 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/09 12:34:34 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// The init_memories function initializes the memory tracking system.
// The add_memory function is used to add a memory allocation to the tracking
// system. If the system is full, it is expanded.
// The free_all_memories function is used to free all memory allocations tracked
// by the system.
// The allocate_user_input function is used to allocate memory for user input.
// The input is tracked by the memory system.

void	init_memories(t_memories *memories,
	t_env *environment, int env_capacity)
{
	memories->capacity = 10;
	memories->size = 0;
	memories->allocations = (void **)calloc(memories->capacity, sizeof(void *));
	if (!memories->allocations)
	{
		printf("Error: Failed to initialize memory tracking system.\n");
		exit(EXIT_FAILURE);
	}
	environment->pairs = (t_key_value *)malloc(env_capacity
			* sizeof(t_key_value));
	if (!environment->pairs)
	{
		printf("Error: Failed to allocate memory for environment.\n");
		exit(EXIT_FAILURE);
	}
	environment->size = 0;
	environment->capacity = env_capacity;
	environment->last_exit_status = 0;
	add_memory(memories, environment->pairs);
}

int	is_in_memories(t_memories *memories, void *ptr)
{
	int	i;

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
	if (is_in_memories(memories, ptr))
	{
		printf("Warning: Pointer %p is already in memories.\n", ptr);
		return ;
	}
	if (memories->size >= memories->capacity)
	{
		memories->capacity *= 2;
		memories->allocations = realloc(memories->allocations,
				memories->capacity * sizeof(void *));
		if (!memories->allocations)
		{
			printf("Error: Failed to expand memory tracking.\n");
			exit(EXIT_FAILURE);
		}
	}
}

void	free_all_memories(t_memories *memories)
{
	int	i;

	i = 0;
	while (i < memories->size)
	{
		free(memories->allocations[i]);
		i++;
	}
	free(memories->allocations);
	memories->size = 0;
	memories->capacity = 0;
}
