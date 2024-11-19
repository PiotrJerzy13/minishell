/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_buildins.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 12:07:25 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/19 14:01:06 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_input_redirection(const char *input_redirect, int *saved_stdin,
	int *last_exit_status)
{
	int	fd_in;

	if (input_redirect)
	{
		fd_in = open(input_redirect, O_RDONLY);
		if (fd_in == -1)
		{
			perror("open failed for input redirection");
			*last_exit_status = 1;
			return (1);
		}
		*saved_stdin = dup(STDIN_FILENO);
		if (*saved_stdin == -1 || dup2(fd_in, STDIN_FILENO) == -1)
		{
			perror("dup failed for input redirection");
			close(fd_in);
			*last_exit_status = 1;
			return (1);
		}
		close(fd_in);
	}
	return (0);
}

int	handle_output_redirection(const char *output_redirect, int append_mode,
			int *saved_stdout, int *last_exit_status)
{
	int	fd_out;

	if (output_redirect)
	{
		if (append_mode)
			fd_out = open(output_redirect, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd_out = open(output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd_out == -1)
		{
			perror("open failed for output redirection");
			*last_exit_status = 1;
			return (1);
		}
		*saved_stdout = dup(STDOUT_FILENO);
		if (*saved_stdout == -1 || dup2(fd_out, STDOUT_FILENO) == -1)
		{
			perror("dup failed for output redirection");
			close(fd_out);
			*last_exit_status = 1;
			return (1);
		}
		close(fd_out);
	}
	return (0);
}

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
	if (clear_output_redirect(command->output_redirect,
			command->input_redirect, exit_st)
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
