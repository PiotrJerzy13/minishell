/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:20:56 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 09:25:58 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

const char	*determine_target_path(char **args, t_env *env, char *old_pwd)
{
	const char	*path;

	if (args[0] == NULL || strcmp(args[0], "~") == 0)
	{
		path = get_env_value("HOME", env);
		if (path == NULL)
		{
			fprintf(stderr, "cd: HOME not set\n");
			return (free(old_pwd), NULL);
		}
	}
	else if (strcmp(args[0], "-") == 0)
	{
		path = get_env_value("OLDPWD", env);
		if (path == NULL)
		{
			fprintf(stderr, "cd: OLDPWD not set\n");
			return (free(old_pwd), NULL);
		}
		printf("%s\n", path);
	}
	else
		path = args[0];
	return (path);
}

int	update_environment_variables(t_env *env, t_memories *mem, char *old_pwd)
{
	char	*new_pwd;

	new_pwd = get_current_directory();
	if (new_pwd == NULL)
	{
		free(old_pwd);
		return (FAILURE);
	}
	add_or_update_env_var(env, "OLDPWD", old_pwd, mem);
	add_or_update_env_var(env, "PWD", new_pwd, mem);
	free(old_pwd);
	free(new_pwd);
	return (SUCCESS);
}

int	bui_cd(char **args, t_env *env, t_memories *memories)
{
	char		*old_pwd;
	const char	*path;

	old_pwd = get_current_directory();
	if (old_pwd == NULL)
	{
		return (FAILURE);
	}
	path = determine_target_path(args, env, old_pwd);
	if (path == NULL)
	{
		return (FAILURE);
	}
	if (change_directory(path, old_pwd) == (FAILURE))
	{
		return (FAILURE);
	}
	return (update_environment_variables(env, memories, old_pwd));
}

int	bui_exit(char **args)
{
	int	exit_code;
	int	i;

	i = 0;
	exit_code = 0;
	if (args[1] != NULL)
	{
		while (args[1][i])
		{
			if (!isdigit(args[1][i]))
			{
				fprintf(stderr, "exit: numeric argument required\n");
				exit(255);
			}
			i++;
		}
		exit_code = atoi(args[1]);
	}
	exit(exit_code);
}
