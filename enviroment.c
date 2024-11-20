/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/20 09:37:14 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// The add_or_update_env_var function is used to add or update an environment
// variable in the t_env structure. If the key already exists, the value is
// updated. If the key does not exist, a new key-value pair is added.
// The unset_env_var function is used to remove an environment variable from the
// t_env structure. The key is used to identify the variable to remove.
// The export_env_var function is used to parse the input string and add
// an environment variable. The input string should be in the format "export KEY
// VALUE". If the key already exists, the value is updated.
// The print_env function is used to print the environment variables.
// Check is the free correctly used in the unset_env_var function.

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

// void	export_env_var(t_env *environment, char *input, t_memories *memories)
// {
// 	char	*key;
// 	char	*value;

// 	key = strtok(input, "=");
// 	value = strtok(NULL, "=");
// 	if (key && value)
// 	{
// 		add_or_update_env_var(environment, key, value, memories);
// 	}
// 	else
// 	{
// 		printf("Error: Invalid format. Use export KEY=VALUE.\n");
// 	}
// }

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

// void	print_env(t_env *env)
// {
// 	int	i;

// 	i = 0;
// 	while (i < env->size)
// 	{
// 		printf("%s=%s\n", env->pairs[i].key, env->pairs[i].value);
// 		i++;
// 	}
// }

void	print_env(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, "OLDPWD") == 0 && env->cd_used_flag == 0)
		{
			i++;
		}
		else
		{
			printf("%s=%s\n", env->pairs[i].key, env->pairs[i].value);
			i++;
		}
	}
}
