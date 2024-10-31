/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/31 15:29:02 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

void	print_env(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->size)
	{
		printf("%s=%s\n", env->pairs[i].key, env->pairs[i].value);
		i++;
	}
}
