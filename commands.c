/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/24 17:33:34 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_executable_path(const char *command)
{
	char	*path_env;
	char	*path;
	char	*dir;
	char	*full_path;

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
		{
			dir = ".";
		}
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
