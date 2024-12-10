/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_enviroment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 16:58:15 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/11 00:17:10 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	calculate_shlvl_length(int number)
{
	int	length;
	int	temp;

	length = 0;
	temp = number;
	if (number < 0)
	{
		temp = -number;
		length++;
	}
	if (temp == 0)
		return (1);
	while (temp > 0)
	{
		temp /= 10;
		length++;
	}
	return (length);
}

char	*int_to_string_with_memory(int number, t_memories *memories)
{
	int		length;
	char	*str;
	int		temp;

	length = calculate_shlvl_length(number);
	str = malloc(length + 1);
	if (!str)
		return (NULL);
	add_memory(memories, str);
	str[length] = '\0';
	temp = number;
	if (temp < 0)
		temp = -temp;
	while (length > 0)
	{
		length--;
		str[length] = '0' + (temp % 10);
		temp /= 10;
	}
	if (number < 0)
		str[0] = '-';
	return (str);
}

void	process_shlvl(char *key, char **value, t_memories *memories)
{
	int	shlvl;

	if (ft_strcmp(key, "SHLVL") == 0)
	{
		shlvl = ft_atoi(*value);
		shlvl++;
		*value = int_to_string_with_memory(shlvl, memories);
		if (!*value)
		{
			perror("Failed to allocate memory for SHLVL");
			exit(EXIT_FAILURE);
		}
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
