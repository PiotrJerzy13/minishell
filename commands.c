/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/10 18:15:17 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**env_to_char_array(t_env *environment)
{
	char	**env_array;
	size_t	len;

	env_array = malloc((environment->size + 1) * sizeof(char *));
	if (!env_array)
	{
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < environment->size; i++)
	{
		len = strlen(environment->pairs[i].key)
			+ strlen(environment->pairs[i].value) + 2;
		env_array[i] = malloc(len);
		if (!env_array[i])
		{
			exit(EXIT_FAILURE);
		}
		snprintf(env_array[i], len, "%s=%s", environment->pairs[i].key,
			environment->pairs[i].value);
	}
	env_array[environment->size] = NULL;
	return (env_array);
}

void	free_env_array(char **env_array)
{
	for (int i = 0; env_array[i] != NULL; i++)
	{
		free(env_array[i]);
	}
	free(env_array);
}

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

int	handle_builtin(t_command *command, t_env *environment, t_memories *memories, int *last_exit_status)
{
	int	result;
	int	saved_stdin;
	int	saved_stdout;
	int	fd_in;

	result = 0;
	saved_stdin = -1;
	saved_stdout = -1;
	if (command->input_redirect)
	{
		fd_in = open(command->input_redirect, O_RDONLY);
		if (fd_in == -1)
		{
			perror("open failed in handle_builtin for input redirection");
			*last_exit_status = 1;
			return (1);
		}
		saved_stdin = dup(STDIN_FILENO);
		if (saved_stdin == -1 || dup2(fd_in, STDIN_FILENO) == -1)
		{
			perror("dup failed in handle_builtin for input redirection");
			close(fd_in);
			*last_exit_status = 1;
			return (1);
		}
		close(fd_in);
	}
	if (command->output_redirect)
	{
		int fd_out = command->append_mode ?
			open(command->output_redirect, O_WRONLY | O_CREAT | O_APPEND, 0644) :
			open(command->output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);

		if (fd_out == -1)
		{
			perror("open failed in handle_builtin for output redirection");
			*last_exit_status = 1;
			return (1);
		}
		saved_stdout = dup(STDOUT_FILENO);
		if (saved_stdout == -1 || dup2(fd_out, STDOUT_FILENO) == -1)
		{
			perror("dup failed in handle_builtin for output redirection");
			close(fd_out);
			*last_exit_status = 1;
			return (1);
		}
		close(fd_out);
	}
	if (strcmp(command->command, "echo") == 0)
	{
		bui_echo(command->args + 1);
		*last_exit_status = 0;
		result = (1);
	}
	else if (strcmp(command->command, "env") == 0)
	{
		print_env(environment);
		*last_exit_status = 0;
		result = 1;
	}
	else if (strcmp(command->command, "export") == 0)
	{
		if (command->args[1] != NULL)
			export_env_var(environment, command->args[1], memories);
		*last_exit_status = 0;
		result = 1;
	}
	else if (strcmp(command->command, "cd") == 0)
	{
		*last_exit_status = (bui_cd(command->args + 1) == -1) ? 2 : 0;
		result = 1;
	}
	else if (strcmp(command->command, "exit") == 0)
	{
		bui_exit(command->args + 1);
		*last_exit_status = 0;
		return (-1);
	}
	else
	{
		*last_exit_status = 127;
		return (0);
	}
	if (saved_stdin != -1)
	{
		if (dup2(saved_stdin, STDIN_FILENO) == -1)
		{
			perror("Failed to restore stdin");
		}
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		if (dup2(saved_stdout, STDOUT_FILENO) == -1)
		{
			perror("Failed to restore stdout");
		}
		close(saved_stdout);
	}
	return (result);
}

void execute_commands(t_command *command_list, int *last_exit_status, t_env *environment)
{
	t_command	*current_command;
	int			pipefd[2];
	int			in_fd;
	pid_t		pid;
	int			status;
	int			builtin_status;
	char		*executable_path;
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
				executable_path = strdup(current_command->command);
			}
			else
			{
				executable_path = find_executable_path(current_command->command);
				if (!executable_path)
				{
					fprintf(stderr, "minishell: %s: command not found\n",
						current_command->command);
					*last_exit_status = 127;
					free_env_array(env_array);
					exit(127);
				}
			}
			execve(executable_path, current_command->args, env_array);
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
			free(executable_path);
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
