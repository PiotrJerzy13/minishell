/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:41:14 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/28 18:01:27 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
