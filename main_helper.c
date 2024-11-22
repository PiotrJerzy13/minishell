/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 18:08:05 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 19:01:00 by pwojnaro         ###   ########.fr       */
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

char	*get_user_input(void)
{
	char	*input;
	char	*line;

	input = NULL;
	line = NULL;
	if (isatty(fileno(stdin)))
		input = readline("minishell> ");
	else
	{
		line = get_next_line(fileno(stdin));
		if (!line)
			return (NULL);
		input = ft_strtrim(line, "\n");
		free(line);
	}
	return (input);
}
