/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_enviroment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 16:58:15 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/31 15:28:34 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	deep_copy_env(t_env const *source_env, t_memories *memories)
{
	t_env	new_env;
	int		i;

	i = 0;
	new_env.size = source_env->size;
	new_env.capacity = source_env->capacity;
	new_env.pairs = malloc(new_env.capacity * sizeof(t_key_value));
	if (!new_env.pairs)
	{
		printf("Error: Failed to allocate memory for environment copy.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, new_env.pairs);
	while (i < source_env->size)
	{
		new_env.pairs[i].key = strdup(source_env->pairs[i].key);
		new_env.pairs[i].value = strdup(source_env->pairs[i].value);
		add_memory(memories, new_env.pairs[i].key);
		add_memory(memories, new_env.pairs[i].value);
		i++;
	}
	return (new_env);
}

t_env	shallow_copy_env(t_env *source_env)
{
	t_env	new_env;

	new_env.size = source_env->size;
	new_env.capacity = source_env->capacity;
	new_env.pairs = source_env->pairs;
	return (new_env);
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

void	copy_environment_to_struct(char **env, t_env *environment,
		t_memories *memories)
{
	int			i;
	char const	*key;
	char const	*value;

	i = 0;
	while (env[i])
	{
		key = strtok(env[i], "=");
		value = strtok(NULL, "=");
		if (key && value)
		{
			add_or_update_env_var(environment, key, value, memories);
		}
		i++;
	}
}
