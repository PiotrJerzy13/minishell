/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 15:00:00 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/09 12:19:58 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// The parse_echo_args function is used to parse the arguments for the echo
// command. It splits the input string into tokens based on spaces and quotes.
// The tokens are stored in an array of strings, which is returned by the funct
// The parse_echo_args function also updates the arg_count variable with the
// number of arguments parsed.
// The handle_builtin function is used to handle built-in commands like echo,
// env, export, unset, pwd, cd, and exit. If the input matches a built-in comman
// the corresponding function is called. The function returns 1 if the command
// a built-in command and 0 otherwise.

int	handle_builtin(t_command *command, t_env *environment, t_memories *memories)
{
	int	result;
	int	saved_stdout;
	int	saved_stdin;
	int	fd_in;
	int	fd_out;

	result = 0;
	saved_stdout = -1;
	saved_stdin = -1;
	fd_in = -1;
	fd_out = -1;
	if (command->input_redirect)
	{
		fd_in = open(command->input_redirect, O_RDONLY);
		if (fd_in == -1)
		{
			perror("open failed in handle_builtin for input redirection");
			return (1);
		}
		saved_stdin = dup(STDIN_FILENO);
		if (dup2(fd_in, STDIN_FILENO) == -1)
		{
			perror("dup2 failed in handle_builtin for input redirection");
			close(fd_in);
			return (1);
		}
		close(fd_in);
	}
	if (command->output_redirect)
	{
		if (command->append_mode)
			fd_out = open(command->output_redirect, O_WRONLY
					| O_CREAT | O_APPEND, 0644);
		else
			fd_out = open(command->output_redirect, O_WRONLY
					| O_CREAT | O_TRUNC, 0644);
		if (fd_out == -1)
		{
			perror("open failed in handle_builtin for output redirection");
			return (1);
		}
		saved_stdout = dup(STDOUT_FILENO);
		if (dup2(fd_out, STDOUT_FILENO) == -1)
		{
			perror("dup2 failed in handle_builtin for output redirection");
			close(fd_out);
			return (1);
		}
		close(fd_out);
	}
	if (strcmp(command->command, "echo") == 0)
	{
		bui_echo(command->args + 1);
		result = 1;
	}
	else if (strcmp(command->command, "env") == 0)
	{
		print_env(environment);
		result = 1;
	}
	else if (strcmp(command->command, "export") == 0)
	{
		if (command->args[1] != NULL)
		{
			export_env_var(environment, command->args[1], memories);
		}
		result = 1;
	}
	else if (strcmp(command->command, "unset") == 0)
	{
		if (command->args[1] != NULL)
		{
			unset_env_var(environment, command->args[1]);
		}
		result = 1;
	}
	else if (strcmp(command->command, "pwd") == 0)
	{
		// bui_pwd();
		// bui_pwd(environment, memories);
		bui_pwd(environment);
		result = 1;
	}
	else if (strcmp(command->command, "cd") == 0)
	{
		// bui_cd(command->args + 1);
		bui_cd(command->args + 1, environment, memories);
		result = 1;
	}
	else if (strcmp(command->command, "exit") == 0)
	{
		bui_exit(command->args + 1);
		result = -1;
	}
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
	return (result);
}

void	start_minishell(void)
{
	printf("Minishell instance started with PID: %d\n", getpid());
}

int	main(int argc, char **argv, char **env)
{
	t_command	*command_list;
	t_env		environment;
	char		*input;
	t_memories	memories;
	t_token		*token_list;
	char		*line;

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
			{
				break ;
			}
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
			tokenize_input(input, &token_list, &memories, &environment);
			parse_input_to_commands(token_list, &command_list, &memories);
			if (command_list && handle_builtin(command_list, &environment,
					&memories) == 1)
			{
			}
			else
			{
				execute_commands(command_list);
			}
		}
		free(input);
		input = NULL;
		command_list = NULL;
		token_list = NULL;
	}
	free_all_memories(&memories);
	return (0);
}
// int	main(int argc, char **argv, char **env)
// {
// 	t_command	*command_list;
// 	t_env		environment;
// 	char		*input;
// 	t_memories	memories;
// 	t_token		*token_list;

// 	(void)argc;
// 	(void)argv;
// 	command_list = NULL;
// 	token_list = NULL;
// 	start_minishell();
// 	if (initialize_shell() == -1)
// 	{
// 		fprintf(stderr, "Failed to initialize shell environment\n");
// 		return (EXIT_FAILURE);
// 	}
// 	signal(SIGQUIT, SIG_IGN);
// 	init_memories(&memories, &environment, 10);
// 	copy_environment_to_struct(env, &environment, &memories);
// 	while (1)
// 	{
// 		input = readline("minishell> ");
// 		if (input == NULL)
// 		{
// 			printf("exit\n");
// 			break ;
// 		}
// 		if (*input)
// 		{
// 			add_history(input);
// 			tokenize_input(input, &token_list, &memories, &environment);
// 			parse_input_to_commands(token_list, &command_list, &memories);
// 			if (command_list && handle_builtin(command_list, &environment,
// 					&memories) == 1)
// 			{
// 			}
// 			else
// 			{
// 				execute_commands(command_list);
// 			}
// 		}
// 		free(input);
// 		input = NULL;
// 		command_list = NULL;
// 		token_list = NULL;
// 	}
// 	free_all_memories(&memories);
// 	return (0);
// }