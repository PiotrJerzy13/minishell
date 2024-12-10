/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_enviroment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 16:58:15 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 17:46:01 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_shlvl(char *key, char **value, t_memories *memories)
{
	int	shlvl;
	int	temp;
	int	length;
	int	is_negative;

	length = 0;
	is_negative = 0;
	if (ft_strcmp(key, "SHLVL") == 0)
	{
		shlvl = ft_atoi(*value);
		free(*value);
		shlvl++;
		if (shlvl < 0)
		{
			is_negative = 1;
			temp = -shlvl;
		}
		else
		{
			temp = shlvl;
		}
		if (temp == 0)
		{
			length = 1;
		}
		else
		{
			while (temp > 0)
			{
				temp /= 10;
				length++;
			}
		}
		if (is_negative)
			length++;
		*value = malloc(length + 1);
		if (!*value)
			return ;
		add_memory(memories, *value);
		temp = shlvl;
		(*value)[length] = '\0';
		while (length > 0)
		{
			length--;
			if (temp < 0)
			{
				(*value)[length] = '0' - (temp % 10);
				temp /= 10;
			}
			else
			{
				(*value)[length] = '0' + (temp % 10);
				temp /= 10;
			}
		}
		if (is_negative)
			(*value)[0] = '-';
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
		equal_sign = ft_strchr(env[i], '=');
		if (equal_sign)
		{
			key = ft_strndup(env[i], equal_sign - env[i], memories);
			value = ft_strndup(equal_sign + 1, ft_strlen(equal_sign + 1),
					memories);
			if (key && value)
				process_shlvl(key, &value, memories);
			if (key && value)
			{
				add_or_update_env_var(environment, key, value, memories);
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

char	*get_env_value(const char *name, t_env *environment,
	t_memories *memories)
{
	size_t	i;

	i = 0;
	while (i < environment->size)
	{
		if (ft_strcmp(environment->pairs[i].key, name) == 0)
		{
			return (ft_strndup(environment->pairs[i].value,
					ft_strlen(environment->pairs[i].value), memories));
		}
		i++;
	}
	return (NULL);
}
