/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 13:46:07 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/19 14:09:37 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	restore_redirections(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1)
			perror("Failed to restore stdin");
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		if (dup2(saved_stdout, STDOUT_FILENO) == -1)
			perror("Failed to restore stdout");
		close(saved_stdout);
	}
}

void	handle_unset(t_command *command, t_env *environment)
{
	int	i;

	i = 1;
	while (command->args[i] != NULL)
	{
		unset_env_var(environment, command->args[i]);
		i++;
	}
}
