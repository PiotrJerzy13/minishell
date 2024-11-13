/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/13 14:26:37 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **env)
{
	t_command	*command_list;
	t_env		environment;
	char		*input;
	t_memories	memories;
	t_token		*token_list;
	char		*line;
	int			last_exit_status;

	last_exit_status = 0;
	(void)argc;
	(void)argv;
	command_list = NULL;
	token_list = NULL;
	if (initialize_shell() == -1)
	{
		fprintf(stderr, "Failed to initialize shell environment\n");
		return (EXIT_FAILURE);
	}
	signal(SIGQUIT, SIG_IGN);
	init_memories(&memories, &environment, 10);
	copy_environment_to_struct(env, &environment, &memories);
	while (1)
	{
		if (isatty(fileno(stdin)))
		{
			input = readline("minishell> ");
		}
		else
		{
			line = get_next_line(fileno(stdin));
			if (line == NULL)
				break ;
			input = ft_strtrim(line, "\n");
			free(line);
		}
		if (input == NULL)
		{
			if (isatty(fileno(stdin)))
				printf("exit\n");
			break ;
		}
		if (*input)
		{
			add_history(input);
			tokenize_input(input, &token_list, &memories, &environment,
				&last_exit_status);
			parse_input_to_commands(token_list, &command_list, &memories);
			if (command_list)
			{
				if (handle_builtin(command_list, &environment, &memories,
						&last_exit_status) == 1)
				{
				}
				else
				{
					execute_commands(command_list, &last_exit_status,
						&environment);
				}
			}
		}
		free(input);
		input = NULL;
		command_list = NULL;
		token_list = NULL;
	}
	free_all_memories(&memories);
	return (last_exit_status);
}
