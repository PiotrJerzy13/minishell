/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struck_pair.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:02:45 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/25 13:26:53 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_key_value(t_key_value_store *store, char *key,
	char *value, t_memories *memories)
{
	if (store->size >= store->capacity)
	{
		store->capacity *= 2;
		store->pairs = realloc(store->pairs,
				store->capacity * sizeof(t_key_value));
		if (!store->pairs)
		{
			printf("Error: Failed to expand key-value store.\n");
			exit(EXIT_FAILURE);
		}
		add_memory(memories, store->pairs);
	}
	store->pairs[store->size].key = strdup(key);
	store->pairs[store->size].value = strdup(value);
	add_memory(memories, store->pairs[store->size].key);
	add_memory(memories, store->pairs[store->size].value);
	store->size++;
}

void	parse_input_to_key_value(t_key_value_store *store, t_memories *memories)
{
	char	*input;
	char	*key;
	char	*value;

	input = allocate_user_input(memories);
	key = strtok(input, "=");
	value = strtok(NULL, "=");
	if (key && value)
	{
		add_key_value(store, key, value, memories);
	}
	else
	{
		printf("Error: Invalid input format. Expected KEY=VALUE.\n");
	}
}

void	init_key_value_store(t_key_value_store *store,
	int initial_capacity, t_memories *memories)
{
	store->size = 0;
	store->capacity = initial_capacity;
	store->pairs = (t_key_value *)malloc(initial_capacity
			* sizeof(t_key_value));
	if (!store->pairs)
	{
		printf("Error: Failed to allocate memory for key-value store.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, store->pairs);
}

void	print_key_value_store(t_key_value_store *store)
{
	int	i;

	i = 0;
	while (i < store->size)
	{
		printf("Key: %s, Value: %s\n", store->pairs[i].key,
			store->pairs[i].value);
		i++;
	}
}
