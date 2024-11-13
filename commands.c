/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/13 15:37:36 by pwojnaro         ###   ########.fr       */
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
			current_command->args = malloc(sizeof(char *) * 10);
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

char	*find_executable_path(const char *command)
{
	char	*path;
	char	*dir;
	char	*path_env;
	char	*full_path;
	size_t	len;

	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	path = strdup(path_env);
	dir = strtok(path, ":");
	while (dir != NULL)
	{
		len = strlen(dir) + strlen(command) + 2;
		full_path = malloc(len);
		if (!full_path)
		{
			free(path);
			return (NULL);
		}
		snprintf(full_path, len, "%s/%s", dir, command);
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

void	execute_commands(t_command *command_list, int *last_exit_status,
	t_env *environment)
{
	t_command	*current_command;
	int			pipefd[2];
	int			in_fd;
	pid_t		pid;
	int			status;
	int			builtin_status;
	char		*exec_path;
	char		**env_array;

	current_command = command_list;
	in_fd = STDIN_FILENO;
	while (current_command)
	{
		builtin_status = handle_builtin(current_command, environment,
				NULL, last_exit_status);
		if (builtin_status == 1)
		{
			current_command = current_command->next;
			continue ;
		}
		else if (builtin_status == -1)
		{
			*last_exit_status = 0;
			break ;
		}
		if (current_command->next)
		{
			if (pipe(pipefd) == -1)
			{
				perror("pipe failed");
				*last_exit_status = 1;
				return ;
			}
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork failed");
			*last_exit_status = 1;
			return ;
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
			env_array = env_to_char_array(environment);
			if (strchr(current_command->command, '/'))
			{
				exec_path = strdup(current_command->command);
			}
			else
			{
				exec_path = find_executable_path(current_command->command);
				if (!exec_path)
				{
					fprintf(stderr, "minishell: %s: command not found\n",
						current_command->command);
					*last_exit_status = 127;
					free_env_array(env_array);
					exit(127);
				}
			}
			execve(exec_path, current_command->args, env_array);
			if (errno == EACCES)
			{
				fprintf(stderr, "minishell: %s: permission denied\n",
					current_command->command);
				*last_exit_status = 126;
			}
			else
			{
				fprintf(stderr, "minishell: %s: execution error\n",
					current_command->command);
				*last_exit_status = 1;
			}
			free(exec_path);
			free_env_array(env_array);
			exit(*last_exit_status);
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
		{
			*last_exit_status = WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status))
		{
			*last_exit_status = 128 + WTERMSIG(status);
		}
	}
}
