/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bui_pwd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 14:09:37 by kkaratsi          #+#    #+#             */
/*   Updated: 2024/11/17 14:09:46 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	bui_pwd(t_env *env)
{
	int	i;

	if (env == NULL || env->pairs == NULL)
	{
		fprintf(stderr, "pwd: PWD not set\n");
		return (FAILURE);
	}
	i = 0;
	while (i < env->size)
	{
		if (strncmp(env->pairs[i].key, "PWD", 3) == 0)
		{
			printf("%s\n", env->pairs[i].value);
			return (SUCCESS);
			break ;
		}
		i++;
	}
	return (FAILURE);
}
