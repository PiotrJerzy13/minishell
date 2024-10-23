/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/22 18:43:27 by pwojnaro         ###   ########.fr       */
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

void	copy_environment_to_struct(char **env, t_env *environment,
	t_memories *memories)
{
	int		i;
	char	*key;
	char	*value;
	char	*key_copy;
	char	*value_copy;

	i = 0;
	while (env[i])
	{
		key = strtok(env[i], "=");
		value = strtok(NULL, "=");
		if (key && value)
		{
			key_copy = strdup(key);
			value_copy = strdup(value);
			add_memory(memories, key_copy);
			add_memory(memories, value_copy);
			add_or_update_env_var(environment, key_copy, value_copy, memories);
		}
		i++;
	}
}

void	add_or_update_env_var(t_env *env, const char *key,
	const char *value, t_memories *memories)
{
	int	i;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, key) == 0)
		{
			free(env->pairs[i].value);
			env->pairs[i].value = strdup(value);
			add_memory(memories, env->pairs[i].value);
			return ;
		}
		i++;
	}
	if (env->size >= env->capacity)
	{
		env->capacity *= 2;
		env->pairs = realloc(env->pairs, env->capacity * sizeof(t_key_value));
		if (!env->pairs)
		{
			printf("Error: Failed to expand environment.\n");
			exit(EXIT_FAILURE);
		}
		add_memory(memories, env->pairs);
	}
	env->pairs[env->size].key = strdup(key);
	env->pairs[env->size].value = strdup(value);
	add_memory(memories, env->pairs[env->size].key);
	add_memory(memories, env->pairs[env->size].value);
	env->size++;
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
