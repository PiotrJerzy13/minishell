/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/10 14:21:12 by kkaratsi         ###   ########.fr       */
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

	current_token = token_list;
	arg_count = 0;
	current_command = NULL;
	while (current_token)
	{
		if (current_command == NULL && current_token->type == TOKEN_COMMAND)
		{
			current_command = init_command_node(memories);
			add_command_node(command_list, current_command);
			current_command->args = malloc(sizeof(char *) * 20);
			add_memory(memories, current_command->args);
			if (strncmp(current_token->value, "./", 2) == 0)
			{
				current_command->command = strdup(current_token->value);
				add_memory(memories, current_command->command);
			}
			else
			{
				current_command->command = strdup(current_token->value);
				add_memory(memories, current_command->command);
			}
			current_command->args[arg_count++] = strdup(current_token->value);
			add_memory(memories, current_command->args[arg_count - 1]);
		}
		else if ((current_token->type == TOKEN_ARGUMENT
				|| current_token->type == TOKEN_COMMAND) && current_command)
		{
			current_command->args[arg_count++] = strdup(current_token->value);
			add_memory(memories, current_command->args[arg_count - 1]);
		}
		else if (current_token->type == TOKEN_OUTPUT_REDIRECT
			&& current_command)
		{
			current_token = current_token->next;
			if (current_token && current_token->type == TOKEN_FILENAME)
			{
				current_command->output_redirect = strdup(current_token->value);
				add_memory(memories, current_command->output_redirect);
			}
		}
		else if (current_token->type == TOKEN_APPEND_OUTPUT_REDIRECT
			&& current_command)
		{
			current_token = current_token->next;
			if (current_token && current_token->type == TOKEN_FILENAME)
			{
				current_command->output_redirect = strdup(current_token->value);
				add_memory(memories, current_command->output_redirect);
				current_command->append_mode = 1;
			}
		}
		else if (current_token->type == TOKEN_INPUT_REDIRECT && current_command)
		{
			current_token = current_token->next;
			if (current_token && current_token->type == TOKEN_FILENAME)
			{
				current_command->input_redirect = strdup(current_token->value);
				add_memory(memories, current_command->input_redirect);
			}
		}
		else if (current_token->type == TOKEN_PIPE && current_command)
		{
			current_command->is_pipe = 1;
			current_command->args[arg_count] = NULL;
			current_command = NULL;
			arg_count = 0;
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
	int			pipefd[2];
	int			in_fd;
	pid_t		pid;
	int			status;

	in_fd = STDIN_FILENO;
	current_command = command_list;
	while (current_command)
	{
		if (current_command->next)
		{
			if (pipe(pipefd) == -1)
			{
				perror("pipe failed");
				exit(EXIT_FAILURE);
			}
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork failed");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			if (in_fd != STDIN_FILENO)
			{
				dup2(in_fd, STDIN_FILENO);
				close(in_fd);
			}
			if (current_command->next)
			{
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
			close(pipefd[0]);
			if (strchr(current_command->command, '/'))
			{
				execv(current_command->command, current_command->args);
			}
			else
			{
				execvp(current_command->command, current_command->args);
			}
			perror("exec failed");
			exit(EXIT_FAILURE);
		}
		else
		{
			if (in_fd != STDIN_FILENO)
				close(in_fd);
			if (current_command->next)
				close(pipefd[1]);
			in_fd = pipefd[0];
			current_command = current_command->next;
		}
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status))
			printf("Process exited with status %d\n", WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			printf("Child process terminated by signal %d\n", WTERMSIG(status));
	}
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
// 	printf("Starting parse_input_to_commands...\n");
// 	while (current_token)
// 	{
// 		if (current_command == NULL && current_token->type == TOKEN_COMMAND)
// 		{
// 			printf("Initializing new command node.\n");
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
// 			printf("Parsed command: %s\n", current_command->command);
// 		}
// 		else if ((current_token->type == TOKEN_ARGUMENT
// 				|| current_token->type == TOKEN_COMMAND) && current_command)
// 		{
// 			current_command->args[arg_count++] = strdup(current_token->value);
// 			add_memory(memories, current_command->args[arg_count - 1]);
// 			printf("Parsed argument %d: %s\n", arg_count - 1,
// 				current_command->args[arg_count - 1]);
// 		}
// 		else if (current_token->type == TOKEN_OUTPUT_REDIRECT
// 			&& current_command)
// 		{
// 			current_token = current_token->next;
// 			if (current_token && current_token->type == TOKEN_FILENAME)
// 			{
// 				current_command->output_redirect = strdup(current_token->value);
// 				add_memory(memories, current_command->output_redirect);
// 				printf("Output redirection set to: %s\n",
// 					current_command->output_redirect);
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
// 				printf("Append output redirection set to: %s\n",
// 					current_command->output_redirect);
// 			}
// 		}
// 		else if (current_token->type == TOKEN_INPUT_REDIRECT && current_command)
// 		{
// 			current_token = current_token->next;
// 			if (current_token && current_token->type == TOKEN_FILENAME)
// 			{
// 				current_command->input_redirect = strdup(current_token->value);
// 				add_memory(memories, current_command->input_redirect);
// 				printf("Input redirection set to: %s\n",
// 					current_command->input_redirect);
// 			}
// 		}
// 		else if (current_token->type == TOKEN_PIPE && current_command)
// 		{
// 			current_command->is_pipe = 1;
// 			printf("Encountered pipe. Setting is_pipe for current command.\n");
// 			current_command->args[arg_count] = NULL;
// 			current_command = NULL;
// 			arg_count = 0;
// 		}
// 		current_token = current_token->next;
// 	}
// 	if (current_command && arg_count < 10)
// 	{
// 		current_command->args[arg_count] = NULL;
// 		printf("Final command args array null-terminated for command: %s\n",
// 			current_command->command);
// 	}
// 	printf("Parsing complete.\n");
// }

// void	execute_commands(t_command *command_list)
// {
// 	t_command	*current_command;
// 	int			pipefd[2];
// 	int			in_fd;
// 	pid_t		pid;
// 	int			status;

// 	in_fd = STDIN_FILENO;
// 	current_command = command_list;
// 	printf("Starting command execution...\n");
// 	while (current_command)
// 	{
// 		if (current_command->next)
// 		{
// 			if (pipe(pipefd) == -1)
// 			{
// 				perror("pipe failed");
// 				exit(EXIT_FAILURE);
// 			}
// 		}
// 		pid = fork();
// 		if (pid == -1)
// 		{
// 			perror("fork failed");
// 			exit(EXIT_FAILURE);
// 		}
// 		else if (pid == 0)
// 		{
// 			if (in_fd != STDIN_FILENO)
// 			{
// 				dup2(in_fd, STDIN_FILENO);
// 				close(in_fd);
// 			}
// 			if (current_command->next)
// 			{
// 				dup2(pipefd[1], STDOUT_FILENO);
// 				close(pipefd[1]);
// 			}
// 			close(pipefd[0]);
// 			if (execvp(current_command->command, current_command->args) == -1)
// 			{
// 				perror("execvp failed");
// 				exit(EXIT_FAILURE);
// 			}
// 		}
// 		else
// 		{
// 			if (in_fd != STDIN_FILENO)
// 				close(in_fd);
// 			if (current_command->next)
// 				close(pipefd[1]);
// 			in_fd = pipefd[0];
// 			current_command = current_command->next;
// 		}
// 	}
// 	while (wait(&status) > 0)
// 	{
// 		if (WIFEXITED(status))
// 			printf("Process exited with status %d\n", WEXITSTATUS(status));
// 		else if (WIFSIGNALED(status))
// 			printf("Child process terminated by signal %d\n", WTERMSIG(status));
// 	}
// 	printf("Command execution complete.\n");
// }
