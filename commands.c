/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 18:27:26 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*construct_and_check_path(const char *dir, const char *command,
	t_memories *memories)
{
	char	*full_path;
	size_t	dir_len;
	size_t	command_len;
	size_t	total_len;

	dir_len = ft_strlen(dir);
	command_len = ft_strlen(command);
	total_len = dir_len + command_len + 2;
	full_path = malloc(total_len);
	if (!full_path)
	{
		perror("malloc failed");
		return (NULL);
	}
	add_memory(memories, full_path);
	ft_strcpy(full_path, dir);
	ft_strcat(full_path, "/");
	ft_strcat(full_path, command);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	return (NULL);
}

char	*find_in_path(const char *command, const char *path_env,
	t_memories *memories)
{
	char	*path;
	char	*full_path;
	char	*dir;

	full_path = NULL;
	path = ft_strndup(path_env, ft_strlen(path_env), memories);
	if (!path)
	{
		perror("ft_strndup failed");
		return (NULL);
	}
	dir = ft_strtok(path, ":");
	while (dir != NULL)
	{
		if (*dir == '\0')
			dir = ".";
		full_path = construct_and_check_path(dir, command, memories);
		if (full_path)
		{
			return (full_path);
		}
		dir = ft_strtok(NULL, ":");
	}
	return (NULL);
}

char	*find_executable_path(const char *command, t_memories *memories)
{
	char	*path_env;

	if (ft_strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
		{
			return (ft_strndup(command, ft_strlen(command), memories));
		}
		return (NULL);
	}
	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	return (find_in_path(command, path_env, memories));
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
