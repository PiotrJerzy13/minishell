/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_enviroment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 16:58:15 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/06 16:26:07 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	copy_environment_to_struct(char **env, t_env *environment,
		t_memories *memories)
{
	int		i;
	char	*equal_sign;
	char	*key;
	char	*value;
	int		shlvl;

	i = 0;
	while (env[i])
	{
		equal_sign = strchr(env[i], '=');
		if (equal_sign)
		{
			key = strndup(env[i], equal_sign - env[i]);
			value = strdup(equal_sign + 1);
			if (key && strcmp(key, "SHLVL") == 0)
			{
				shlvl = atoi(value);
				free(value);
				shlvl++;
				value = malloc(12);
				if (value)
					snprintf(value, 12, "%d", shlvl);
			}
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
