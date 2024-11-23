/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:36:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:37:52 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
