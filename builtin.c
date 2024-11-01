/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:20:56 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/01 17:35:11 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// The chdir function in C is used to change the current working directory
// of the calling process.
// Error handling not implemented using perror() function.
// Using atoi() and other build function for example strlen strcmp.
// The getcwd function in C is used to obtain the current working directory.
// Do we want to use whole Libft functions or just some of them?

int	bui_cd(char **args)
{
	const char	*path;

	if (args[1] == NULL)
	{
		path = getenv("HOME");
		if (path == NULL)
		{
			fprintf(stderr, "cd: HOME not set\n");
			return (1);
		}
	}
	else
	{
		path = args[1];
	}
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

int	bui_echo(char **args)
{
	int	i;
	int	newline;

	i = 0;
	newline = 1;
	if (args[i] && strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		write(1, args[i], strlen(args[i]));
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
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
				i++;
			}
		}
		exit_code = atoi(args[1]);
	}
	exit(exit_code);
}

int	bui_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("getcwd");
		return (1);
	}
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}
