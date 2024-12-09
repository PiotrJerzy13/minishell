/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_commands.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 12:07:25 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 14:59:46 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_simple_command(t_command *command, int *last_exit_status)
{
	int	exit_code;

	if (strcmp(command->command, "echo") == 0)
	{
		bui_echo(command->args + 1);
		*last_exit_status = 0;
		return (1);
	}
	else if (strcmp(command->command, "exit") == 0)
	{
		if (command->args[1])
		{
			exit_code = atoi(command->args[1]);
			if (exit_code < 0 || !isdigit(*command->args[1]))
			{
				fprintf(stderr, "exit: numeric argument required\n");
				exit(255);
			}
			*last_exit_status = exit_code % 256;
			exit(exit_code);
		}
		*last_exit_status = 0;
		exit(0);
	}
	return (0);
}

int	validate_export_argument(const char *arg)
{
	if (!arg || strchr(arg, '=') == NULL)
	{
		fprintf(stderr, "export: `%s': not a valid identifier\n", arg);
		return (0);
	}
	return (1);
}

int	handle_env_var_commands(t_command *command, t_env *environment,
	int *last_exit_status)
{
	if (strcmp(command->command, "export") == 0)
	{
		if (bui_export(&environment, command->args) == FAILURE)
			*last_exit_status = 1;
		else
			*last_exit_status = 0;
		return (1);
	}
	if (strcmp(command->command, "unset") == 0)
	{
		if (command->args[1] != NULL)
		{
			handle_unset(command, environment);
			*last_exit_status = 0;
		}
		else
		{
			*last_exit_status = 1;
		}
		return (1);
	}
	return (0);
}

int	handle_directory_commands(t_command *command, t_env *environment,
	t_memories *memories, int *last_exit_status)
{
	if (strcmp(command->command, "cd") == 0)
	{
		if (bui_cd(command->args + 1, environment, memories) == FAILURE)
			*last_exit_status = 2;
		else
			*last_exit_status = 0;
		return (1);
	}
	return (0);
}

int	handle_environment_command(t_command *command, t_env *environment,
	t_memories *memories, int *last_exit_status)
{
	if (handle_env_var_commands(command, environment, last_exit_status))
		return (1);
	if (handle_directory_commands(command, environment, memories,
			last_exit_status))
		return (1);
	return (0);
}
