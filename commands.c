/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/03 12:42:28 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// The init_command_node function allocates memory for a new command.
// It should put it to memories struckt. Have to modified it.
// The add_command_node function is used to build a linked list of commands.
// ls -l | grep "txt" | wc -l  add command node for each command.
// The parse_input_to_commands function is used to parse the input tokens
// lots of printf for debugging.Will be removed.
// Tokenization: The input string is split into tokens, and each token is
// assigned a type from e_token_type based on its role (e.g., commands,
// arguments, pipes, redirections).
// Building s_command Nodes: For each new command (marked by TOKEN_COMMAND 
// or TOKEN_PIPE), a new s_command node is created and populated with arguments 
// and redirection tokens. The is_pipe flag in s_command is set when encounter
// TOKEN_PIPE, indicating that the current command’s output should pipe into the
// next command.
//  execute_commands iterates over the s_command linked list. It uses the fields
// in each s_command node (command name, arguments, redirections) to execute eac
// command, handling pipes and redirections as specified in the struct.
// pid_t pid: Stores the process ID of the child process created by fork()
// int status: Stores the exit status of the child process
// int pipefd[2]: Stores the file descriptors for the pipe
// int in_fd: Stores the file descriptor for the input of the current command
// dup2(oldfd, newfd): This function duplicates oldfd to newfd, closing newfd
// The execvp() function is used to execute the command specified in the first
// replacing the current process image with a new one
// The waitpid() function is used to wait for the child process to finish
// The WIFSIGNALED macro checks if the child process was terminated by a signal
// The WTERMSIG macro returns the signal number that terminated the child proces
// The close() function is used to close the file descriptors for the pipe
// signal(SIGQUIT, SIG_DFL) is used to set the signal handler for SIGQUIT to the

t_command	*init_command_node(t_memories *memories)
{
	t_command	*new_node;

	new_node = (t_command *)malloc(sizeof(t_command));
	if (!new_node)
	{
		printf("Error: Failed to allocate memory for command node.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, new_node);
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
			current_command = init_command_node(memories);
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

	current_command = command_list;
	in_fd = 0;
	printf("Starting command execution...\n");
	while (current_command)
	{
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
			signal(SIGQUIT, SIG_DFL);
			printf("In child process for command: %s\n",
				current_command->command);
			if (in_fd != 0)
			{
				dup2(in_fd, 0);
				close(in_fd);
			}
			if (current_command->is_pipe)
			{
				dup2(pipefd[1], 1);
				close(pipefd[1]);
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
			if (WIFSIGNALED(status) && WTERMSIG(status) == SIGQUIT)
			{
				printf("Process for command %s was terminated by SIGQUIT\n",
					current_command->command);
			}
			if (current_command->is_pipe)
			{
				close(pipefd[1]);
				in_fd = pipefd[0];
			}
			else
			{
				in_fd = 0;
			}
			current_command = current_command->next;
		}
	}
	printf("Command execution complete.\n");
}
