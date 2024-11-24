/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_commands.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 12:07:25 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/24 17:26:27 by pwojnaro         ###   ########.fr       */
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

void	print_declared_env(t_env *env)
{
	size_t	i;

	i = 0;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, "OLDPWD") == 0 && env->cd_used_flag == 0)
		{
			printf("declare -x OLDPWD\n");
		}
		else
		{
			printf("declare -x %s=\"%s\"\n", env->pairs[i].key,
				env->pairs[i].value);
		}
		i++;
	}
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

int	bui_export(t_env **env, char **args)
{
	int	i;

	if (!args[1])
	{
		print_declared_env(*env);
		return (SUCCESS);
	}
	i = 1;
	while (args[i])
	{
		export_env_var(*env, args[i], (*env)->memories);
		i++;
	}
	return (SUCCESS);
}

int	handle_environment_command(t_command *command, t_env *environment,
	t_memories *memories, int *last_exit_status)
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
			fprintf(stderr, "unset: not enough arguments\n");
			*last_exit_status = 1;
		}
		return (1);
	}
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
