/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 13:46:07 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/06 13:50:15 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_unset(t_command *command, t_env *environment)
{
	size_t	i;

	i = 1;
	while (command->args[i] != NULL)
	{
		unset_env_var(environment, command->args[i]);
		i++;
	}
}

void	print_env(t_env *env)
{
	size_t	i;

	i = 0;
	while (i < env->size)
	{
		printf("%s=%s\n", env->pairs[i].key, env->pairs[i].value);
		i++;
	}
}

void	unset_env_var(t_env *env, const char *key)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(key, "PATH") == 0)
		{
			env->path_unset = 1; // Set the flag indicating PATH is unset
		}
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
