/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 09:38:02 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_executable_path(const char *command)
{
	char	*path_env;
	char	*path;
	char	*dir;
	char	*full_path;

	if (strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));
		return (NULL);
	}
	path_env = getenv("PATH");
	if (!path_env)
	{
		fprintf(stderr, "Error: PATH environment variable is not set.\n");
		return (NULL);
	}
	path = strdup(path_env);
	if (!path)
	{
		perror("strdup failed");
		return (NULL);
	}
	dir = strtok(path, ":");
	while (dir != NULL)
	{
		if (*dir == '\0')
			dir = ".";
		if (asprintf(&full_path, "%s/%s", dir, command) == -1)
		{
			perror("asprintf failed");
			free(path);
			return (NULL);
		}
		if (access(full_path, X_OK) == 0)
		{
			free(path);
			return (full_path);
		}
		free(full_path);
		dir = strtok(NULL, ":");
	}
	free(path);
	return (NULL);
}
int	bui_echo(char **args)
{
	int	i;
	int	newline;

	if (args == NULL || args[0] == NULL)
	{
		fprintf(stderr, "\n");
		return (1);
	}
	i = check_n_flag(args, &newline);
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

void	print_declared_env(t_env *env)
{
	size_t	i;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, "OLDPWD") == 0 && env->cd_used_flag == 0)
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
