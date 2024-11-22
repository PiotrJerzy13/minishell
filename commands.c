/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 16:43:13 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_command_node(t_command **head, t_command *new_command)
{
	t_command	*current;

	current = *head;
	if (!current)
	{
		*head = new_command;
		return ;
	}
	while (current->next)
		current = current->next;
	current->next = new_command;
}

// void	parse_input_to_commands(t_token *token_list, t_command **command_list,
// 	t_memories *memories)
// {
// 	t_command	*current_command;
// 	t_token		*current_token;
// 	int			arg_count;

// 	current_token = token_list;
// 	arg_count = 0;
// 	current_command = NULL;
// 	while (current_token)
// 	{
// 		if (current_command == NULL && current_token->type == TOKEN_COMMAND)
// 		{
// 			current_command = init_command_node(memories);
// 			add_command_node(command_list, current_command);
// 			current_command->args = malloc(sizeof(char *) * 10);
// 			add_memory(memories, current_command->args);
// 			if (strncmp(current_token->value, "./", 2) == 0)
// 			{
// 				current_command->command = strdup(current_token->value);
// 				add_memory(memories, current_command->command);
// 			}
// 			else
// 			{
// 				current_command->command = strdup(current_token->value);
// 				add_memory(memories, current_command->command);
// 			}
// 			current_command->args[arg_count++] = strdup(current_token->value);
// 			add_memory(memories, current_command->args[arg_count - 1]);
// 		}
// 		else if ((current_token->type == TOKEN_ARGUMENT
// 				|| current_token->type == TOKEN_COMMAND) && current_command)
// 		{
// 			current_command->args[arg_count++] = strdup(current_token->value);
// 			add_memory(memories, current_command->args[arg_count - 1]);
// 		}
// 		else if (current_token->type == TOKEN_HEREDOC && current_command)
// 		{
// 			current_token = current_token->next;
// 			if (current_token && current_token->type == TOKEN_FILENAME)
// 			{
// 				current_command->heredoc_list = NULL;
// 				if (collect_heredoc_input(current_token->value,
// 						&current_command->heredoc_list) == -1)
// 				{
// 					fprintf(stderr, "Error collecting heredoc input\n");
// 					return ;
// 				}
// 			}
// 		}
// 		else if (current_token->type == TOKEN_OUTPUT_REDIRECT
// 			&& current_command)
// 		{
// 			current_token = current_token->next;
// 			if (current_token && current_token->type == TOKEN_FILENAME)
// 			{
// 				current_command->output_redirect = strdup(current_token->value);
// 				add_memory(memories, current_command->output_redirect);
// 			}
// 		}
// 		else if (current_token->type == TOKEN_APPEND_OUTPUT_REDIRECT
// 			&& current_command)
// 		{
// 			current_token = current_token->next;
// 			if (current_token && current_token->type == TOKEN_FILENAME)
// 			{
// 				current_command->output_redirect = strdup(current_token->value);
// 				add_memory(memories, current_command->output_redirect);
// 				current_command->append_mode = 1;
// 			}
// 		}
// 		else if (current_token->type == TOKEN_INPUT_REDIRECT && current_command)
// 		{
// 			current_token = current_token->next;
// 			if (current_token && current_token->type == TOKEN_FILENAME)
// 			{
// 				current_command->input_redirect = strdup(current_token->value);
// 				add_memory(memories, current_command->input_redirect);
// 			}
// 		}
// 		else if (current_token->type == TOKEN_PIPE && current_command)
// 		{
// 			current_command->is_pipe = 1;
// 			current_command->args[arg_count] = NULL;
// 			current_command = NULL;
// 			arg_count = 0;
// 		}
// 		current_token = current_token->next;
// 	}
// 	if (current_command && arg_count < 10)
// 	{
// 		current_command->args[arg_count] = NULL;
// 	}
// }

char	*find_executable_path(const char *command)
{
	char	*path_env;
	char	*path;
	char	*dir;
	char	*full_path;

	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	path = strdup(path_env);
	if (!path)
		return (NULL);
	dir = strtok(path, ":");
	while (dir != NULL)
	{
		asprintf(&full_path, "%s/%s", dir, command);
		if (access(full_path, X_OK) == 0)
		{
			free(path);
			return (full_path);
		}
		free(full_path);
		dir = strtok(NULL, ":");
	}
	free(path);
	return (NULL);
}

int	handle_heredoc_redirection(t_command *command, int *last_exit_status)
{
	int				heredoc_pipe[2];
	t_heredoc_node	*current_node;

	if (pipe(heredoc_pipe) == -1)
	{
		perror("[ERROR] Pipe for heredoc failed");
		*last_exit_status = 1;
		return (-1);
	}
	current_node = command->heredoc_list;
	while (current_node)
	{
		write(heredoc_pipe[1], current_node->line, strlen(current_node->line));
		write(heredoc_pipe[1], "\n", 1);
		current_node = current_node->next;
	}
	close(heredoc_pipe[1]);
	return (heredoc_pipe[0]);
}

int	setup_pipes(int *pipefd, int *last_exit_status)
{
	if (pipe(pipefd) == -1)
	{
		perror("pipe failed");
		*last_exit_status = 1;
		return (-1);
	}
	return (0);
}

t_command	*initialize_command(t_token *current_token,
		t_command **command_list, t_memories *memories)
{
	t_command	*command;

	command = init_command_node(memories);
	add_command_node(command_list, command);
	command->args = malloc(sizeof(char *) * 10);
	add_memory(memories, command->args);
	command->command = strdup(current_token->value);
	add_memory(memories, command->command);
	command->args[0] = strdup(current_token->value);
	add_memory(memories, command->args[0]);
	return (command);
}
