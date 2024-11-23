/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_initializer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 18:08:05 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:45:59 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_shell_state(t_shell_state *state)
{
	state->command_list = NULL;
	state->token_list = NULL;
	state->last_exit_status = 0;
	state->input = NULL;
	state->memories.allocations = NULL;
	state->memories.size = 0;
	state->memories.capacity = 0;
	state->environment.pairs = NULL;
	state->environment.size = 0;
	state->environment.capacity = 0;
	state->environment.memories = &state->memories;
}

int	initialize_shell_environment(t_memories *memories,
	t_env *environment, char **env)
{
	if (initialize_shell() == -1)
	{
		fprintf(stderr, "Failed to initialize shell environment\n");
		return (-1);
	}
	signal(SIGQUIT, SIG_IGN);
	init_memories(memories, environment, 10);
	copy_environment_to_struct(env, environment, memories);
	return (0);
}

t_token_context	init_token_context(t_command_context *context)
{
	t_token_context	token_context;

	token_context.token_list = context->token_list;
	token_context.memories = context->memories;
	token_context.environment = context->environment;
	token_context.last_exit_status = context->last_exit_status;
	token_context.expect_filename = 0;
	return (token_context);
}

t_command_context	*create_command_context(t_shell_state *state)
{
	t_command_context	*context;

	context = malloc(sizeof(t_command_context));
	if (!context)
	{
		fprintf(stderr, "Error: Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}
	context->token_list = &state->token_list;
	context->command_list = &state->command_list;
	context->memories = &state->memories;
	context->environment = &state->environment;
	context->last_exit_status = &state->last_exit_status;
	return (context);
}
