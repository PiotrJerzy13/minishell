/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 15:29:23 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*construct_and_check_path(const char *dir, const char *command)
{
	char	*full_path;

	if (asprintf(&full_path, "%s/%s", dir, command) == -1)
	{
		perror("asprintf failed");
		return (NULL);
	}
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}

char	*find_in_path(const char *command, const char *path_env)
{
	char	*path;
	char	*full_path;
	char	*dir;

	path = strdup(path_env);
	full_path = NULL;
	dir = strtok(path, ":");
	while (dir != NULL)
	{
		if (*dir == '\0')
			dir = ".";
		full_path = construct_and_check_path(dir, command);
		if (full_path)
		{
			free(path);
			return (full_path);
		}
		dir = strtok(NULL, ":");
	}
	free(path);
	return (NULL);
}

char	*find_executable_path(const char *command)
{
	char	*path_env;

	if (ft_strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));
		return (NULL);
	}
	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	return (find_in_path(command, path_env));
}

void	print_declared_env(t_env *env)
{
	size_t	i;

	i = 0;
	while (i < env->size)
	{
		if (ft_strcmp(env->pairs[i].key, "OLDPWD") == 0 && env->cd_flag == 0)
		{
			printf("declare -x OLDPWD\n");
		}
		else
		{
			printf("declare -x %s=\"%s\"\n", env->pairs[i].key,
				env->pairs[i].value);
		}
		i++;
	}
}

int	bui_export(t_env **env, char **args)
{
	int	i;

	if (!args[1])
	{
		print_declared_env(*env);
		return (SUCCESS);
	}
	i = 1;
	while (args[i])
	{
		export_env_var(*env, args[i], (*env)->memories);
		i++;
	}
	return (SUCCESS);
}
