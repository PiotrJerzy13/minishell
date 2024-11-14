/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: piotr <piotr@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 15:20:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/14 20:41:13 by piotr            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	skip_spaces(char **input)
{
	while (**input && isspace(**input))
		(*input)++;
}

char	*get_env_value(const char *name, t_env *environment)
{
	int	i;

	i = 0;
	while (i < environment->size)
	{
		if (strcmp(environment->pairs[i].key, name) == 0)
		{
			return (strdup(environment->pairs[i].value));
		}
		i++;
	}
	return (NULL);
}

char	*ft_strtrim(char *str, const char *set)
{
	size_t	start;
	size_t	end;
	char	*trimmed;

	start = 0;
	end = strlen(str);
	while (str[start] && strchr(set, str[start]))
		start++;
	while (end > start && strchr(set, str[end - 1]))
		end--;
	trimmed = malloc(end - start + 1);
	if (!trimmed)
		return (NULL);
	strncpy(trimmed, str + start, end - start);
	trimmed[end - start] = '\0';
	return (trimmed);
}

char	*get_next_line(int fd)
{
	char	*line;
	int		i;
	char	ch;

	line = malloc(1024);
	i = 0;
	if (!line)
		return (NULL);
	while (read(fd, &ch, 1) > 0 && ch != '\n')
	{
		line[i++] = ch;
		if (i >= 1023)
			break ;
	}
	line[i] = '\0';
	if (i == 0 && ch != '\n')
	{
		free(line);
		return (NULL);
	}
	else
	{
		return (line);
	}
}

int is_same_file(const char *file1, const char *file2)
{
    struct stat stat1, stat2;

    if (stat(file1, &stat1) == -1 || stat(file2, &stat2) == -1)
        return (0);
    return (stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino);
}

int handle_builtin(t_command *command, t_env *environment,
                   t_memories *memories, int *last_exit_status)
{
    int result = 0;
    int saved_stdin = -1;
    int saved_stdout = -1;
    int fd_in, fd_out;

    if (command->input_redirect && command->output_redirect &&
        is_same_file(command->input_redirect, command->output_redirect)) 
    {
        // Clear the output file if input and output are the same
        fd_out = open(command->output_redirect, O_WRONLY | O_TRUNC, 0644);
        if (fd_out == -1) {
            perror("open failed in handle_builtin for clearing output file");
            *last_exit_status = 1;
            return 1;
        }
        close(fd_out);
    }

    // Handle input redirection
    if (command->input_redirect) {
        fd_in = open(command->input_redirect, O_RDONLY);
        if (fd_in == -1) {
            perror("open failed in handle_builtin for input redirection");
            *last_exit_status = 1;
            return 1;
        }
        saved_stdin = dup(STDIN_FILENO);
        if (saved_stdin == -1 || dup2(fd_in, STDIN_FILENO) == -1) {
            perror("dup failed in handle_builtin for input redirection");
            close(fd_in);
            *last_exit_status = 1;
            return 1;
        }
        close(fd_in);
    }

    // Handle output redirection
    if (command->output_redirect) {
        if (command->append_mode) {
            fd_out = open(command->output_redirect, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd_out = open(command->output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (fd_out == -1) {
            perror("open failed in handle_builtin for output redirection");
            *last_exit_status = 1;
            return 1;
        }
        saved_stdout = dup(STDOUT_FILENO);
        if (saved_stdout == -1 || dup2(fd_out, STDOUT_FILENO) == -1) {
            perror("dup failed in handle_builtin for output redirection");
            close(fd_out);
            *last_exit_status = 1;
            return 1;
        }
        close(fd_out);
    }

    // Handle the specific built-in commands
    if (strcmp(command->command, "echo") == 0) {
        bui_echo(command->args + 1);
        *last_exit_status = 0;
        result = 1;
    } else if (strcmp(command->command, "env") == 0) {
        print_env(environment);
        *last_exit_status = 0;
        result = 1;
    } else if (strcmp(command->command, "export") == 0) {
        if (command->args[1] != NULL) {
            export_env_var(environment, command->args[1], memories);
        }
        *last_exit_status = 0;
        result = 1;
    } else if (strcmp(command->command, "cd") == 0) {
        if (bui_cd(command->args + 1) == -1) {
            *last_exit_status = 2;
        } else {
            *last_exit_status = 0;
        }
        result = 1;
    } else if (strcmp(command->command, "exit") == 0) {
        bui_exit(command->args + 1);
        *last_exit_status = 0;
        return -1;
    } else {
        *last_exit_status = 127;
        return 0;
    }

    // Restore original stdin and stdout if they were redirected
    if (saved_stdin != -1) {
        if (dup2(saved_stdin, STDIN_FILENO) == -1) {
            perror("Failed to restore stdin");
        }
        close(saved_stdin);
    }
    if (saved_stdout != -1) {
        if (dup2(saved_stdout, STDOUT_FILENO) == -1) {
            perror("Failed to restore stdout");
        }
        close(saved_stdout);
    }
    return result;
}