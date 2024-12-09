/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_initializer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 18:08:05 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 22:45:42 by pwojnaro         ###   ########.fr       */
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
	state->environment.memories = &state->memories;
}

int	initialize_shell_environment(t_memories *memories,
	t_env *environment, char **env)
{
	if (initialize_shell() == -1)
	{
		return (-1);
	}
	init_memories(memories, environment, 10);
	copy_environment_to_struct(env, environment, memories);
	return (0);
}
