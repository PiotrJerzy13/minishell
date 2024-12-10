/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 17:18:53 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 13:58:45 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	setup_redirections(t_command *current_command, int in_fd)
{
	if (handle_input_redirection2(current_command, in_fd) == -1)
		return (-1);
	if (handle_output_redirection2(current_command) == -1)
		return (-1);
	return (0);
}

void	handle_pipe_redirection3(t_command *current_command,
			t_execution_context *context)
{
	if (current_command->next)
	{
		if (dup2(context->pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2 failed for pipe output");
			exit(1);
		}
		close(context->pipe_fd[1]);
		close(context->pipe_fd[0]);
	}
}

void	execute_env_command(t_execution_context *context)
{
	size_t	i;

	i = 0;
	while (i < context->environment->size)
	{
		if (context->environment->pairs[i].key
			&& context->environment->pairs[i].value)
		{
			printf("%s=%s\n", context->environment->pairs[i].key,
				context->environment->pairs[i].value);
		}
		i++;
	}
	exit(0);
}

void	execute_child_process(t_command *current_command,
		t_execution_context *context)
{
	signal(SIGQUIT, SIG_DFL);
	if (setup_redirections(current_command, context->in_fd) == -1)
		exit(1);
	handle_pipe_redirection3(current_command, context);
	if (ft_strcmp(current_command->command, "env") == 0)
	{
		execute_env_command(context);
	}
	exe_command(current_command, context->environment,
		context->last_exit_status);
	exit(*context->last_exit_status);
}

void	handle_parent_process(t_command *current_command, int *in_fd,
			int *pipe_fd)
{
	if (*in_fd != STDIN_FILENO)
	{
		close(*in_fd);
	}
	if (current_command->next)
	{
		close(pipe_fd[1]);
		*in_fd = pipe_fd[0];
	}
	else
	{
		*in_fd = STDIN_FILENO;
	}
}
