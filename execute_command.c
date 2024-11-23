/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:34:40 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:40:57 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exe_command(t_command *command,
	t_env *environment, int *last_exit_status)
{
	char	**env_array;
	char	*exec_path;

	env_array = env_to_char_array(environment);
	exec_path = find_executable_path(command->command);
	if (!exec_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", command->command);
		*last_exit_status = 127;
		free_env_array(env_array);
		exit(127);
	}
	execve(exec_path, command->args, env_array);
	perror("[ERROR] execve failed");
	free_env_array(env_array);
	exit(1);
}

int	is_last_command(t_command *command)
{
	return (command->next == NULL);
}

int	handle_heredoc_if_needed(t_command *command, int in_fd,
	int *last_exit_status)
{
	if (command->heredoc_list)
	{
		in_fd = handle_heredoc_redirection(command, last_exit_status);
	}
	return (in_fd);
}

void	execute_commands(t_command *command_list,
		int *last_exit, t_env *environment)
{
	t_command		*current_command;
	t_exec_context	context;
	int				pipefd[2];
	int				in_fd;

	current_command = command_list;
	in_fd = STDIN_FILENO;
	context.last_exit_status = last_exit;
	context.environment = environment;
	while (current_command)
	{
		in_fd = handle_heredoc_if_needed(current_command, in_fd, last_exit);
		if (in_fd == -1)
			return ;
		if (setup_pipe_if_needed(current_command, pipefd, last_exit) == -1)
			return ;
		context.in_fd = in_fd;
		context.pipefd = pipefd;
		handle_command_execution(current_command, &context);
		in_fd = prepare_next_command(pipefd, current_command);
		current_command = current_command->next;
	}
	wait_for_children(last_exit);
}

void	handle_command_execution(t_command *command, t_exec_context *context)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		*(context->last_exit_status) = 1;
		return ;
	}
	else if (pid == 0)
	{
		setup_child_redirections(context->in_fd, context->pipefd,
			is_last_command(command));
		exe_command(command, context->environment,
			context->last_exit_status);
	}
	else
	{
		handle_parent_cleanup(context->in_fd, context->pipefd, command);
	}
}

int	prepare_next_command(int *pipefd, t_command *command)
{
	if (is_last_command(command))
		return (STDIN_FILENO);
	else
		return (pipefd[0]);
}

void	add_argument_to_command(t_token *current_token,
	t_command *current_command, t_memories *memories, int *arg_count)
{
	current_command->args[*arg_count] = strdup(current_token->value);
	add_memory(memories, current_command->args[*arg_count]);
	(*arg_count)++;
}
