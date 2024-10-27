/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/27 14:15:44 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**parse_echo_args(char *input, int *arg_count)
{
	char	**args;
	char	*token;
	char	*start;
	char	quote_char;

	*arg_count = 0;
	args = malloc(sizeof(char *) * 20);
	while (*input)
	{
		while (*input && isspace(*input))
			input++;
		if (*input == '"' || *input == '\'')
		{
			quote_char = *input++;
			start = input;
			while (*input && *input != quote_char)
				input++;
			if (*input == quote_char)
			{
				token = strndup(start, input - start);
				input++;
			}
			else
			{
				printf("Error: unmatched quote\n");
				free(args);
				return (NULL);
			}
		}
		else
		{
			start = input;
			while (*input && !isspace(*input))
				input++;
			token = strndup(start, input - start);
		}
		args[(*arg_count)++] = token;
	}
	args[*arg_count] = NULL;
	return (args);
}

int	main(int argc, char **argv, char **env)
{
	t_command	*command_list;
	t_env		environment;
	char		*input;
	t_memories	memories;
	t_token		*token_list;
	int			arg_count;
	char		**args;
	int 		i;

	(void)argc;
	(void)argv;
	i = 0;
	command_list = NULL;
	token_list = NULL;
	init_memories(&memories, &environment, 10);
	copy_environment_to_struct(env, &environment, &memories);
	while (1)
	{
		printf("minishell> ");
		input = allocate_user_input(&memories);
		if (strncmp(input, "echo", 4) == 0)
		{
			args = parse_echo_args(input + 5, &arg_count);
			if (args)
			{
				bui_echo(args);
				while (i < arg_count)
				{
					free(args[i]);
					i++;
				}
				free(args);
			}
		}
		else if (strcmp(input, "env") == 0)
		{
			print_env(&environment);
		}
		else if (strncmp(input, "export", 6) == 0)
		{
			export_env_var(&environment, input + 7, &memories);
		}
		else
		{
			parse_input_to_commands(token_list, &command_list, &memories);
			execute_commands(command_list, &environment, &memories);
		}
		token_list = NULL;
	}
	free_all_memories(&memories);
	return (0);
}
