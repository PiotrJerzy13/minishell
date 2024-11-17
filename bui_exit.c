/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bui_exit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 14:11:10 by kkaratsi          #+#    #+#             */
/*   Updated: 2024/11/17 14:11:26 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
