/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/25 14:27:49 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	allocate_environment(t_env *environment, int size, t_memories *memories)
{
	environment->pairs = (t_key_value *)calloc(size + 1, sizeof(t_key_value));
	if (!environment->pairs)
	{
		return (1);
	}
	environment->size = 0;
	environment->capacity = size;
	add_memory(memories, environment->pairs);
	return (0);
}

char	*allocate_user_input(t_memories *memories)
{
	int		capacity;
	char	*input;
	int		length;
	int		ch;

	capacity = INITIAL_CAPACITY;
	input = malloc(capacity * sizeof(char));
	if (!input)
	{
		printf("Error: Failed to allocate memory for user input.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, input);
	length = 0;
	ch = getchar();
	while (ch != '\n' && ch != EOF)
	{
		if (length >= capacity - 1)
		{
			capacity *= 2;
			input = realloc(input, capacity * sizeof(char));
			if (!input)
			{
				printf("Error: Failed to expand memory for user input.\n");
				exit(EXIT_FAILURE);
			}
			add_memory(memories, input);
		}
		input[length++] = (char) ch;
		ch = getchar();
	}
	input[length] = '\0';
	return (input);
}

void	unset_env_var(t_env *env, const char *key)
{
	int	i;
	int	j;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, key) == 0)
		{
			free(env->pairs[i].key);
			free(env->pairs[i].value);
			j = i;
			while (j < env->size - 1)
			{
				env->pairs[j] = env->pairs[j + 1];
				j++;
			}
			env->size--;
			return ;
		}
		i++;
	}
}
