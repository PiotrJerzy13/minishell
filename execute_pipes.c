/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:50:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 17:24:13 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_pipe(t_command **current_command, int *arg_count)
{
	if (*current_command)
	{
		(*current_command)->is_pipe = 1;
		(*current_command)->args[*arg_count] = NULL;
		*current_command = NULL;
		*arg_count = 0;
	}
	else
	{
		printf("Error: Pipe '|' without preceding command.\n");
	}
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

int	setup_pipes(t_command *current_command, int *pipe_fd, int *last_exit_status)
{
	if (current_command->next)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("Pipe creation failed");
			*last_exit_status = 1;
			return (-1);
		}
	}
	return (0);
}
