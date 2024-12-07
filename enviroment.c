/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: piotr <piotr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/07 20:25:36 by piotr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	export_env_var(t_env *environment, char *input, t_memories *memories)
{
	char	*key;
	char	*value;

	if (!validate_export_argument(input))
		return ;
	key = strtok(input, "=");
	value = strtok(NULL, "=");
	if (key && value)
		add_or_update_env_var(environment, key, value, memories);
	else
		fprintf(stderr, "Error: Invalid format. Use export KEY=VALUE.\n");
}

char	**env_to_char_array(t_env *environment)
{
	char	**env_array;
	size_t	i;

	env_array = malloc((environment->size + 1) * sizeof(char *));
	if (!env_array)
	{
		perror("Failed to allocate environment array");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (i < environment->size)
	{
		env_array[i] = malloc(strlen(environment->pairs[i].key)
				+ strlen(environment->pairs[i].value) + 2);
		if (!env_array[i])
		{
			perror("Failed to allocate environment string");
			exit(EXIT_FAILURE);
		}
		snprintf(env_array[i], strlen(environment->pairs[i].key)
			+ strlen(environment->pairs[i].value) + 2,
			"%s=%s", environment->pairs[i].key, environment->pairs[i].value);
		i++;
	}
	env_array[environment->size] = NULL;
	return (env_array);
}

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
	size_t	i;

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

void	free_env_array(char **env_array)
{
	size_t	i;

	i = 0;
	while (env_array[i] != NULL)
	{
		free(env_array[i]);
		i++;
	}
	free(env_array);
}

char	*get_env_value(const char *name, t_env *environment)
{
	size_t	i;

	i = 0;
	while (i < environment->size)
	{
		if (strcmp(environment->pairs[i].key, name) == 0)
		{
			return (strdup(environment->pairs[i].value));
		}
		i++;
	}
	return (NULL);
}
