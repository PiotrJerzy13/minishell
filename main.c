/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/04 11:28:07 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

void	process_commands(char *input, t_command_context *context)
{
	t_token_context	token_context;
	// int				parse_status;
	// Check if PATH is unset and return immediately if it is
    if (context->environment->path_unset == 1)
    {
        printf("ls: No such file or directory\n");
        return;
    }
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
			printf("exit\n");
			break ;
		}
		if (*state.input == '\0')
		{
			continue ;
		}
		process_commands(state.input, context);
		free(state.input);
	}
	free_all_memories(&state.memories);
	free(context);
	return (state.last_exit_status);
}
