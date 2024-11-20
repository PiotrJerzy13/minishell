/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bui_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 15:39:26 by kkaratsi          #+#    #+#             */
/*   Updated: 2024/11/20 09:26:05 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Validation of Identifiers: Ensure the key is a valid identifier.
// Handling Appending Values : Check if the key ends with += and handle accordingly.
// Error Handling: Print an error message if the key is not a valid identifier.

void	print_declared_env(t_env *env)
{
	int		i;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, "OLDPWD") == 0 && env->cd_used_flag == 0)
		{
			printf("declare -x OLDPWD\n");
			i++;
			continue ;
		}
		else
		{
			printf("declare -x ");
			printf("%s=\"%s\"\n", env->pairs[i].key, env->pairs[i].value);
			i++;
		}
	}
}

int	bui_export(t_env **env, char **args)
{
	int	i;

	i = 0;
	while (args[i] != NULL)
	{
		i++;
	}
	if (i == 1)
	{
		print_declared_env(*env);
		return (SUCCESS);
	}
	else
	{
		export_env_var(*env, args[i], (*env)->memories);
	}
	return (SUCCESS);
}
