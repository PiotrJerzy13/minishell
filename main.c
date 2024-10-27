/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/27 09:51:36 by pwojnaro         ###   ########.fr       */
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
	t_env		copied_env;

	(void)argc;
	(void)argv;
	command_list = NULL;
	token_list = NULL;
	init_memories(&memories, &environment, 10);
	copy_environment_to_struct(env, &environment, &memories);
	while (1)
	{
		printf("minishell> ");
		input = allocate_user_input(&memories);
		tokenize_input(input, &token_list);
		if (strcmp(input, "env") == 0)
		{
			print_env(&environment);
		}
		else if (strncmp(input, "export", 6) == 0)
		{
			export_env_var(&environment, input + 7, &memories);
		}
		else if (strncmp(input, "deep_copy_env", 13) == 0)
		{
			copied_env = deep_copy_env(&environment, &memories);
			free_env(&copied_env);
		}
		else
		{
			parse_input_to_commands(&command_list, &memories);
			execute_commands(command_list, &environment, &memories);
			free_command_list(command_list);
		}
	}
	return (0);
}
