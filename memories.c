/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memories.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:49:26 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/25 16:30:53 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	add_memory(memories, environment->pairs);
}

void	add_memory(t_memories *memories, void *ptr)
{
	void	**new_allocations;

	if (!ptr)
		return ;
	if (memories->size >= memories->capacity)
	{
		memories->capacity *= 2;
		new_allocations = (void **)realloc(memories->allocations,
				memories->capacity * sizeof(void *));
		if (!new_allocations)
		{
			printf("Error: Failed to expand memory tracking array.\n");
			exit(EXIT_FAILURE);
		}
		memories->allocations = new_allocations;
	}
	memories->allocations[memories->size++] = ptr;
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
