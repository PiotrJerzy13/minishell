/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_commands.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 12:07:25 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 17:10:18 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_simple_command(t_command *command, t_env *environment,
	int *last_exit_status)
{
	if (strcmp(command->command, "echo") == 0)
	{
		bui_echo(command->args + 1);
		*last_exit_status = 0;
		return (1);
	}
	else if (strcmp(command->command, "env") == 0)
	{
		print_env(environment);
		*last_exit_status = 0;
		return (1);
	}
	else if (strcmp(command->command, "exit") == 0)
	{
		bui_exit(command->args + 1);
		*last_exit_status = 0;
		return (-1);
	}
	return (0);
}

int	handle_environment_command(t_command *command, t_env *environment,
	t_memories *memories, int *last_exit_status)
{
	if (strcmp(command->command, "export") == 0)
	{
		if (command->args[1] != NULL)
			export_env_var(environment, command->args[1], memories);
		*last_exit_status = 0;
		return (1);
	}
	if (strcmp(command->command, "unset") == 0)
	{
		handle_unset(command, environment);
		*last_exit_status = 0;
		return (1);
	}
	if (strcmp(command->command, "cd") == 0)
	{
		if (bui_cd(command->args + 1) == -1)
			*last_exit_status = 2;
		else
			*last_exit_status = 0;
		return (1);
	}
	return (0);
}

int	handle_builtin(t_command *command, t_env *environment, t_memories *memories,
	int *exit_st)
{
	int	saved_stdin;
	int	saved_stdout;
	int	result;

	saved_stdin = -1;
	saved_stdout = -1;
	if (clear_output_redirect(command->output_redirect, exit_st)
		|| handle_input_redirection(command->input_redirect,
			&saved_stdin, exit_st)
		|| handle_output_redirection(command->output_redirect,
			command->append_mode, &saved_stdout, exit_st))
	{
		restore_redirections(saved_stdin, saved_stdout);
		return (1);
	}
	result = handle_simple_command(command, environment, exit_st)
		|| handle_environment_command(command, environment, memories, exit_st);
	if (!result)
		*exit_st = 127;
	restore_redirections(saved_stdin, saved_stdout);
	return (result);
}
