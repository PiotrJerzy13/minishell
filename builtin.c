/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:20:56 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 14:26:49 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	bui_cd(char **args)
{
	const char	*path;

	if (args[0] == NULL)
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
		path = args[0];
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
		printf("%s", args[i]);
		if (args[i + 1])
		{
			printf(" ");
		}
		fflush(stdout);
		i++;
	}
	if (newline)
		printf("\n");
	fflush(stdout);
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
