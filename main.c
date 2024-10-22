/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/14 16:38:20 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **env)
{
	t_command	*command_list;
	t_env_node	*env_list;
	char		*input;
	char		*key;
	char		*value;
	t_memories	memories;
	t_token		*token_list;

	(void)argc;
	(void)argv;
	token_list = NULL;
	command_list = NULL;
	env_list = NULL;
	init_memories(&memories);
	copy_environment_to_list(env, &env_list, &memories);
	while (1)
	{
		printf("minishell> ");
		input = allocate_user_input(&memories);
		tokenize_input(input, &token_list);
		if (strcmp(input, "env") == 0)
		{
			print_env(env_list);
		}
		else if (strncmp(input, "export", 6) == 0)
		{
			key = strtok(input + 7, "=");
			value = strtok(NULL, "=");
			add_or_update_env_var(&env_list, key, value);
		}
		else
		{
            // Execute commands stored in command_list
		}
		free_all_memories(&memories);
	}
	return (0);
}
