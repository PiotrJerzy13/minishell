/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/22 18:56:14 by pwojnaro         ###   ########.fr       */
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

	(void)argc;
	(void)argv;
	token_list = NULL;
	command_list = NULL;
	init_memories(&memories);
	init_env(&environment, 10, &memories);
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
			export_env_var(&environment, input + 7);
		}
		else
		{
			parse_input(token_list, &command_list, &memories);
			execute_commands(command_list, &environment, &memories);
		}
		free_all_memories(&memories);
	}
	return (0);
}
