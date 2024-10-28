/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/28 17:59:58 by pwojnaro         ###   ########.fr       */
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

void	parse_input_to_commands(t_token *token_list, t_command **command_list,
	t_memories *memories)
{
	t_command	*current_command;
	t_token		*current_token;
	int			arg_count;

	current_command = NULL;
	current_token = token_list;
	arg_count = 0;
	printf("Starting parse_input_to_commands...\n");
	while (current_token)
	{
		if (current_command == NULL && current_token->type == TOKEN_COMMAND)
		{
			printf("Initializing new command node.\n");
			current_command = init_command_node();
			add_memory(memories, current_command);
			add_command_node(command_list, current_command);
			current_command->args = malloc(sizeof(char *) * 10);
			add_memory(memories, current_command->args);
			printf("Args array initialized.\n");
			current_command->command = strdup(current_token->value);
			add_memory(memories, current_command->command);
			current_command->args[arg_count++] = strdup(current_token->value);
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
		else if (current_token->type == TOKEN_PIPE && current_command)
		{
			current_command->is_pipe = 1;
			printf("Encountered pipe. Setting is_pipe for current command.\n");
			current_command->args[arg_count] = NULL;
			printf("Final command args array null-terminated for command: %s\n",
				current_command->command);
			current_command = NULL;
			arg_count = 0;
		}
		current_token = current_token->next;
	}
	if (current_command && arg_count < 10)
	{
		current_command->args[arg_count] = NULL;
		printf("Final command args array null-terminated for command: %s\n",
			current_command->command);
	}
	printf("Parsing complete.\n");
}

void	execute_commands(t_command *command_list)
	{
	t_command	*current_command;
	pid_t		pid;
	int			status;
	int			pipefd[2];
	int			in_fd;
	int			i;

	current_command = command_list;
	in_fd = STDIN_FILENO;
	printf("Starting command execution...\n");
	while (current_command)
	{
		i = 0;
		if (current_command->is_pipe)
		{
			if (pipe(pipefd) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			printf("Created a pipe for command: %s\n",
				current_command->command);
		}
		pid = fork();
		if (pid == 0)
		{
			printf("In child process for command: %s\n",
				current_command->command);
			if (in_fd != STDIN_FILENO)
			{
				dup2(in_fd, STDIN_FILENO);
				close(in_fd);
			}
			if (current_command->is_pipe)
			{
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
			printf("Executing command: %s\n", current_command->command);
			while (current_command->args[i])
			{
				printf("Arg[%d]: %s\n", i, current_command->args[i]);
				i++;
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
			printf("In parent process, waiting for child: %s\n",
				current_command->command);
			waitpid(pid, &status, 0);
			if (current_command->is_pipe)
			{
				close(pipefd[1]);
				in_fd = pipefd[0];
			}
			else
			{
				in_fd = STDIN_FILENO;
			}
			printf("Process for command %s finished with status %d\n",
				current_command->command, status);
			current_command = current_command->next;
		}
	}
	printf("Command execution complete.\n");
}
