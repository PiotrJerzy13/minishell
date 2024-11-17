/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bui_echo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 14:10:21 by kkaratsi          #+#    #+#             */
/*   Updated: 2024/11/17 14:10:42 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_n_flag(char **args, int *newline)
{
	int	i;
	int	j;

	*newline = 1;
	i = 0;
	while (args[i])
	{
		if (args[i][0] == '-' && args[i][1] == 'n')
		{
			j = 1;
			while (args[i][j] == 'n')
				j++;
			if (args[i][j] == '\0')
			{
				*newline = 0;
				i++;
			}
			else
				break ;
		}
		else
			break ;
	}
	return (i);
}

int	bui_echo(char **args)
{
	int	i;
	int	newline;

	if (args == NULL || args[0] == NULL)
	{
		fprintf(stderr, "bui_echo: args is NULL\n");
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
