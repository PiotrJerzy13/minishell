/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_builtins.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 14:57:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 15:08:56 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_redirections2(t_command *command, int *saved_stdin,
		int *saved_stdout, int *exit_st)
{
	if (command->output_redirect && !command->append_mode)
	{
		if (clear_output_redirect(command->output_redirect, exit_st))
			return (1);
	}
	if (command->input_redirect)
	{
		if (handle_input_redirection(command->input_redirect,
				saved_stdin, exit_st))
			return (1);
	}
	if (command->output_redirect)
	{
		if (handle_output_redirection(command->output_redirect,
				command->append_mode, saved_stdout, exit_st))
			return (1);
	}
	return (0);
}

int	execute_builtin_command(t_command *command, t_env *environment,
	t_memories *memories, int *exit_st)
{
	int	result;

	result = handle_simple_command(command, exit_st)
		|| handle_environment_command(command, environment, memories, exit_st);
	if (!result)
		*exit_st = 127;
	return (result);
}

int	handle_builtin(t_command *command, t_env *environment,
	t_memories *memories, int *exit_st)
{
	int	saved_stdin;
	int	saved_stdout;
	int	result;

	saved_stdin = -1;
	saved_stdout = -1;
	if (handle_redirections2(command, &saved_stdin, &saved_stdout, exit_st))
	{
		restore_redirections(saved_stdin, saved_stdout);
		return (1);
	}
	result = execute_builtin_command(command, environment, memories, exit_st);
	restore_redirections(saved_stdin, saved_stdout);
	return (result);
}
