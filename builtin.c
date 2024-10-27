/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:20:56 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/27 09:47:44 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	bui_cd(void)
{
	printf("Executing cd\n");
	return (0);
}

int	bui_echo(char **args)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (args[0] && strcmp(args[0], "-n") == 0)
	{
		newline = 0;
		i = 1;
	}
	while (args[i])
	{
		write(STDOUT_FILENO, args[i], strlen(args[i]));
		if (args[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}

int	bui_env(void)
{
	printf("Executing env\n");
	return (0);
}

int	bui_exit(void)
{
	printf("Executing exit\n");
	return (0);
}

int	bui_export(void)
{
	printf("Executing export\n");
	return (0);
}

int bui_pwd(void)
{
	printf("Executing pwd\n");
	return (0);
}

int	bui_unset(void)
{
	printf("Executing unset\n");
	return (0);
}
