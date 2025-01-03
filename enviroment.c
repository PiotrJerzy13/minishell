/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 22:09:50 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	export_env_var(t_env *environment, char *input, t_memories *memories)
{
	char	*key;
	char	*value;

	if (!validate_export_argument(input))
		return ;
	key = ft_strtok(input, "=");
	value = ft_strtok(NULL, "=");
	if (key && value)
		add_or_update_env_var(environment, key, value, memories);
	else
		printf("Error: Invalid format. Use export KEY=VALUE.\n");
}

char	**env_to_char_array(t_env *environment, t_memories *memories)
{
	char	**env_array;
	size_t	i;

	i = 0;
	env_array = malloc((environment->size + 1) * sizeof(char *));
	if (!env_array)
		exit(EXIT_FAILURE);
	add_memory(memories, env_array);
	while (i < environment->size)
	{
		env_array[i] = str_concat(environment->pairs[i].key,
				environment->pairs[i].value, "=", memories);
		if (!env_array[i])
			exit(EXIT_FAILURE);
		add_memory(memories, env_array[i]);
		i++;
	}
	env_array[environment->size] = NULL;
	return (env_array);
}

void	update_env_var(t_env *env, int index, const char *value,
		t_memories *memories)
{
	if (is_in_memories(memories, env->pairs[index].value))
		remove_memory(memories, env->pairs[index].value);
	env->pairs[index].value = ft_strndup(value, ft_strlen(value), memories);
	if (!env->pairs[index].value)
	{
		perror("Failed to update environment variable value");
		exit(EXIT_FAILURE);
	}
}

void	add_env_var(t_env *env, const char *key, const char *value,
	t_memories *memories)
{
	size_t		new_capacity;
	t_key_value	*new_pairs;
	size_t		i;

	i = 0;
	if (env->size >= env->capacity)
	{
		new_capacity = env->capacity * 2;
		new_pairs = malloc(new_capacity * sizeof(t_key_value));
		if (!new_pairs)
			exit(EXIT_FAILURE);
		while (i < env->size)
		{
			new_pairs[i] = env->pairs[i];
			i++;
		}
		add_memory(memories, new_pairs);
		env->pairs = new_pairs;
		env->capacity = new_capacity;
	}
	env->pairs[env->size].key = ft_strndup(key, ft_strlen(key), memories);
	env->pairs[env->size].value = ft_strndup(value, ft_strlen(value), memories);
	if (!env->pairs[env->size].key || !env->pairs[env->size].value)
		exit(EXIT_FAILURE);
	env->size++;
}

void	add_or_update_env_var(t_env *env, const char *key, const char *value,
			t_memories *memories)
{
	size_t	i;

	i = 0;
	while (i < env->size)
	{
		if (ft_strcmp(env->pairs[i].key, key) == 0)
		{
			update_env_var(env, i, value, memories);
			return ;
		}
		i++;
	}
	add_env_var(env, key, value, memories);
}
