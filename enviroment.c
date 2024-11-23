/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:39:59 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * export_env_var - Adds or updates an environment variable 
 * in a custom environment structure.
 *
 * This function takes a string input in the format "KEY=VALUE" 
 * and updates the custom environment structure (t_env) 
 * by either adding a new key-value pair or updating an 
 * existing one. If the input is invalid (e.g., not in 
 * the correct format), an error 
 * message is displayed.
 */

void	export_env_var(t_env *environment, char *input, t_memories *memories)
{
	char	*key;
	char	*value;

	key = strtok(input, "=");
	value = strtok(NULL, "=");
	if (key && value)
	{
		add_or_update_env_var(environment, key, value, memories);
	}
	else
	{
		printf("Error: Invalid format. Use export KEY=VALUE.\n");
	}
}
/**
 * env_to_char_array - Converts a custom environment structure 
 * into a standard C-style array.
 *
 * This function transforms a custom environment structure (t_env) into a 
 * format compatible with standard C-style environment arrays. Each environment 
 * variable is represented as a string in the format "key=value". This type of 
 * array is commonly used with system calls like execve that require environment 
 * variables to be passed as a char ** array.
 */

char	**env_to_char_array(t_env *environment)
{
	char	**env_array;
	size_t	len;
	int		i;

	i = 0;
	env_array = malloc((environment->size + 1) * sizeof(char *));
	if (!env_array)
	{
		exit(EXIT_FAILURE);
	}
	while (i < environment->size)
	{
		len = strlen(environment->pairs[i].key)
			+ strlen(environment->pairs[i].value) + 2;
		env_array[i] = malloc(len);
		if (!env_array[i])
		{
			exit(EXIT_FAILURE);
		}
		snprintf(env_array[i], len, "%s=%s", environment->pairs[i].key,
			environment->pairs[i].value);
		i++;
	}
	env_array[environment->size] = NULL;
	return (env_array);
}

//Used when you need to modify the value of an existing key without 
//changing its position or key name.
void	update_env_var(t_env *env, int index, const char *value)
{
	free(env->pairs[index].value);
	env->pairs[index].value = strdup(value);
	if (!env->pairs[index].value)
		exit(EXIT_FAILURE);
}

//Used when you need to add a new key-value pair to the environment.
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
/**
 * add_or_update_env_var - Adds a new environment variable 
 * or updates an existing one.
 *
 * This function ensures that a key-value pair is added or updated in the 
 * custom environment structure (`t_env`). If the key already exists, its value 
 * is updated using `update_env_var`. If the key does not exist, it is added 
 * as a new key-value pair using `add_env_var`
 */

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

void	free_env_array(char **env_array)
{
	int	i;

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
	int	i;

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
