/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/28 13:45:53 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*init_command_node(void)
{
	t_command	*new_node;

	new_node = (t_command *)malloc(sizeof(t_command));
	if (!new_node)
	{
		printf("Error: Failed to allocate memory for command node.\n");
		exit(EXIT_FAILURE);
	}
	new_node->command = NULL;
	new_node->args = NULL;
	new_node->is_pipe = 0;
	new_node->input_redirect = NULL;
	new_node->output_redirect = NULL;
	new_node->next = NULL;
	return (new_node);
}

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

void	free_command_list(t_command *head)
{
	t_command	*tmp;
	int			i;

	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp->command);
		if (tmp->args)
		{
			i = 0;
			while (tmp->args[i])
				free(tmp->args[i++]);
			free(tmp->args);
		}
		free(tmp->input_redirect);
		free(tmp->output_redirect);
		free(tmp);
	}
}

void	parse_input_to_commands(t_token *token_list, t_command **command_list,
	t_memories *memories)
{
	t_command	*current_command;
	t_token		*current_token;
	int			arg_count;

	current_command = NULL;
	current_token = token_list;
	arg_count = 0;
	while (current_token)
	{
		if (current_command == NULL && current_token->type == TOKEN_COMMAND)
		{
			current_command = malloc(sizeof(t_command));
			add_memory(memories, current_command);
			current_command->command = strdup(current_token->value);
			add_memory(memories, current_command->command);
			current_command->args = malloc(sizeof(char *) * 10);
			add_memory(memories, current_command->args);
			current_command->args[0] = strdup(current_token->value);
			add_memory(memories, current_command->args[0]);
			arg_count = 1;
			current_command->is_pipe = 0;
			current_command->input_redirect = NULL;
			current_command->output_redirect = NULL;
			current_command->next = NULL;
			add_command_node(command_list, current_command);
			printf("Parsed command: %s\n", current_command->command);
		}
		else if ((current_token->type == TOKEN_ARGUMENT || current_token->type
				== TOKEN_COMMAND) && current_command)
		{
			current_command->args[arg_count] = strdup(current_token->value);
			add_memory(memories, current_command->args[arg_count]);
			printf("Parsed argument %d: %s\n", arg_count,
				current_command->args[arg_count]);
			arg_count++;
		}
		else if (current_token->type == TOKEN_INPUT_REDIRECT && current_command)
		{
			current_token = current_token->next;
			if (current_token)
			{
				current_command->input_redirect = strdup(current_token->value);
				add_memory(memories, current_command->input_redirect);
				printf("Parsed input redirection: %s\n",
					current_command->input_redirect);
			}
		}
		else if ((current_token->type == TOKEN_OUTPUT_REDIRECT
				|| current_token->type == TOKEN_APPEND_OUTPUT_REDIRECT)
			&& current_command)
		{
			current_token = current_token->next;
			if (current_token)
			{
				current_command->output_redirect = strdup(current_token->value);
				add_memory(memories, current_command->output_redirect);
				current_command->is_pipe = (current_token->type
						== TOKEN_APPEND_OUTPUT_REDIRECT);
				printf("Parsed output redirection: %s (append: %d)\n",
					current_command->output_redirect, current_command->is_pipe);
			}
		}
		current_token = current_token->next;
	}
	if (current_command && arg_count < 10)
	{
		current_command->args[arg_count] = NULL;
	}
}

void	execute_commands(t_command *command_list)
{
	t_command	*current_command;
	pid_t		pid;
	int			status;
	int			i;
	int			fd;
	int			open_flags;

	i = 0;
	current_command = command_list;
	while (current_command)
	{
		printf("Executing command: %s\n", current_command->command);
		while (current_command->args[i])
		{
			printf("Arg[%d]: %s\n", i, current_command->args[i]);
			i++;
		}
		pid = fork();
		if (pid == 0)
		{
			if (current_command->input_redirect)
			{
				fd = open(current_command->input_redirect, O_RDONLY);
				if (fd < 0)
				{
					perror("open input");
					exit(EXIT_FAILURE);
				}
				dup2(fd, STDIN_FILENO);
				close(fd);
			}
			if (current_command->output_redirect)
			{
				open_flags = O_WRONLY | O_CREAT;
				if (current_command->is_pipe)
				{
					open_flags |= O_APPEND;
				}
				else
				{
					open_flags |= O_TRUNC;
				}
				fd = open(current_command->output_redirect, open_flags, 0644);
				if (fd < 0)
				{
					perror("open output");
					exit(EXIT_FAILURE);
				}
				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
			if (execvp(current_command->command, current_command->args) == -1)
			{
				perror("execvp");
				exit(EXIT_FAILURE);
			}
		}
		else if (pid < 0)
		{
			perror("fork");
			return ;
		}
		else
		{
			waitpid(pid, &status, 0);
		}
		current_command = current_command->next;
	}
}
