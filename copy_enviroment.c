/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_enviroment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 16:58:15 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 14:55:00 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_env_var(t_env *env, int index, const char *value)
{
	free(env->pairs[index].value);
	env->pairs[index].value = strdup(value);
	if (!env->pairs[index].value)
		exit(EXIT_FAILURE);
}

void	add_env_var(t_env *env, const char *key, const char *value,
	t_memories *memories)
{
	if (env->size >= env->capacity)
	{
		env->capacity *= 2;
		env->pairs = realloc(env->pairs, env->capacity * sizeof(t_key_value));
		if (!env->pairs)
			exit(EXIT_FAILURE);
		add_memory(memories, env->pairs);
	}
	env->pairs[env->size].key = strdup(key);
	env->pairs[env->size].value = strdup(value);
	if (!env->pairs[env->size].key || !env->pairs[env->size].value)
		exit(EXIT_FAILURE);
	add_memory(memories, env->pairs[env->size].key);
	add_memory(memories, env->pairs[env->size].value);
	env->size++;
}

void	add_or_update_env_var(t_env *env, const char *key, const char *value,
	t_memories *memories)
{
	int	i;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, key) == 0)
		{
			update_env_var(env, i, value);
			return ;
		}
		i++;
	}
	add_env_var(env, key, value, memories);
}

void	copy_environment_to_struct(char **env, t_env *environment,
		t_memories *memories)
{
	int		i;
	char	*equal_sign;
	char	*key;
	char	*value;

	i = 0;
	while (env[i])
	{
		equal_sign = strchr(env[i], '=');
		if (equal_sign)
		{
			key = strndup(env[i], equal_sign - env[i]);
			value = strdup(equal_sign + 1);
			if (key && value)
			{
				add_or_update_env_var(environment, key, value, memories);
				free(key);
				free(value);
			}
		}
		i++;
	}
}
