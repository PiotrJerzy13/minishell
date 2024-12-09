/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_enviroment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 16:58:15 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 12:24:21 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_shlvl(char *key, char **value)
{
	int	shlvl;

	if (strcmp(key, "SHLVL") == 0)
	{
		shlvl = atoi(*value);
		free(*value);
		shlvl++;
		*value = malloc(12);
		if (*value)
			snprintf(*value, 12, "%d", shlvl);
	}
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
				process_shlvl(key, &value);
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
