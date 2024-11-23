/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 12:33:45 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_commands(char *input, t_command_context *context)
{
	t_token_context	token_context;

	if (*input)
	{
		add_history(input);
		token_context = init_token_context(context);
		tokenize_input(input, &token_context);
		parse_input_to_commands(*(context->token_list), context->command_list,
			context->memories);
		if (*(context->command_list))
		{
			if (handle_builtin(*(context->command_list), context->environment,
					context->memories, context->last_exit_status) == 0)
			{
				execute_commands(*(context->command_list),
					context->last_exit_status, context->environment);
			}
		}
		*(context->command_list) = NULL;
		*(context->token_list) = NULL;
	}
}

int	main(int argc, char **argv, char **env)
{
	t_shell_state		state;
	t_command_context	*context;

	(void)argc;
	(void)argv;
	init_shell_state(&state);
	if (initialize_shell_environment(&state.memories,
			&state.environment, env) == -1)
		return (EXIT_FAILURE);
	context = create_command_context(&state);
	while (1)
	{
		state.input = get_user_input();
		if (!state.input)
		{
			if (isatty(fileno(stdin)))
				printf("exit\n");
			break ;
		}
		process_commands(state.input, context);
		free(state.input);
	}
	free_all_memories(&state.memories);
	free(context);
	return (state.last_exit_status);
}
