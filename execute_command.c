/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:34:40 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/11 00:46:07 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exe_command(t_command *command, t_env *environment,
	int *last_exit_status)
{
	char	**env_array;
	char	*exec_path;

	env_array = env_to_char_array(environment, environment->memories);
	exec_path = find_executable_path(command->command, environment->memories);
	if (!exec_path)
	{
		printf("minishell: %s: command not found\n", command->command);
		*last_exit_status = 127;
		exit(127);
	}
	execve(exec_path, command->args, env_array);
	perror("execve failed");
	exit(1);
}

void	add_argument_to_command(const t_token *current_token,
	t_command *current_command,
	t_memories *memories, int *arg_count)
{
	current_command->args[*arg_count] = ft_strndup(current_token->value,
			ft_strlen(current_token->value), memories);
	if (!current_command->args[*arg_count])
	{
		perror("Error: Failed to allocate memory for argument.");
		return ;
	}
	(*arg_count)++;
}

void	wait_for_children(int *last_exit_status)
{
	int	status;

	while (wait(&status) > 0)
	{
		if (WIFEXITED(status))
		{
			*last_exit_status = WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status))
		{
			*last_exit_status = 128 + WTERMSIG(status);
		}
	}
}

int	process_command2(t_command *current_command, t_execution_context *context)
{
	pid_t	pid;

	if (current_command->heredoc_list)
	{
		if (handle_heredoc2(current_command, &context->in_fd,
				context->last_exit_status) == -1)
			return (-1);
	}
	if (setup_pipes(current_command, context->pipe_fd,
			context->last_exit_status) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		perror("Fork failed");
		*context->last_exit_status = 1;
		return (-1);
	}
	if (pid == 0)
		execute_child_process(current_command, context);
	signal(SIGQUIT, SIG_IGN);
	handle_parent_process(current_command, &context->in_fd, context->pipe_fd);
	return (0);
}

void	execute_commands(t_command *command_list, int *last_exit_status,
			t_env *environment)
{
	t_command			*current_command;
	t_execution_context	context;

	current_command = command_list;
	context.in_fd = STDIN_FILENO;
	context.last_exit_status = last_exit_status;
	context.environment = environment;
	while (current_command)
	{
		if (process_command2(current_command, &context) == -1)
			return ;
		current_command = current_command->next;
	}
	wait_for_children(context.last_exit_status);
}
