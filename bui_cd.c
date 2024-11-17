/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bui_cd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 13:47:25 by kkaratsi          #+#    #+#             */
/*   Updated: 2024/11/17 14:18:03 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_current_directory(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd == NULL)
	{
		perror("getcwd");
	}
	return (cwd);
}

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

int	change_directory(const char *path, char *old_pwd)
{
	if (chdir(path) != 0)
	{
		perror("cd");
		free(old_pwd);
		return (FAILURE);
	}
	return (SUCCESS);
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
