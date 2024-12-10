/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:50:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 20:31:18 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_pipe(t_command **current_command, int *arg_count)
{
	if (*current_command)
	{
		(*current_command)->is_pipe = 1;
		(*current_command)->args[*arg_count] = NULL;
		*current_command = NULL;
		*arg_count = 0;
	}
	else
	{
		printf("Error: Pipe '|' without preceding command.\n");
	}
}

void	handle_echo_with_pid(const char *arg)
{
	const char	*dollar_pid;

	dollar_pid = "$$";
	if (ft_strcmp(arg, dollar_pid))
	{
		printf("%d\n", getpid());
	}
	if (ft_strcmp(arg, dollar_pid) == 0)
	{
		printf("%d", getpid());
	}
}

char	*process_escape_sequences(const char *input)
{
	size_t		len;
	char		*result;
	char		*dst;

	len = strlen(input);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	dst = result;
	while (*input)
	{
		if (*input == '\\' && (*(input + 1) == 'n' || *(input + 1) == 't'))
		{
			if (*(input + 1) == 'n')
				*dst++ = '\n';
			else if (*(input + 1) == 't')
				*dst++ = '\t';
			input += 2;
		}
		else
			*dst++ = *input++;
	}
	*dst = '\0';
	return (result);
}

void	handle_echo_with_e_flag(char **args, int start_index)
{
	char	*processed_arg;
	int		i;

	i = start_index;
	while (args[i])
	{
		processed_arg = process_escape_sequences(args[i]);
		if (processed_arg)
		{
			printf("%s", processed_arg);
			free(processed_arg);
		}
		if (args[i + 1])
			printf(" ");
		i++;
	}
	printf("\n");
}

void	handle_normal_echo(char **args, int start_index)
{
	int	i;

	i = start_index;
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	printf("\n");
}

int	bui_echo(char **args)
{
	int	i;
	int	newline;

	if (args == NULL || args[0] == NULL)
	{
		fprintf(stderr, "\n");
		return (1);
	}
	i = check_n_flag(args, &newline);
	if (ft_strcmp(args[0], "-e") == 0)
	{
		handle_echo_with_e_flag(args, i + 1);
		return (0);
	}
	if (args[1] && strcmp(args[0], "$$") == 0)
	{
		handle_echo_with_pid(args[1]);
	}
	else
	{
		handle_normal_echo(args, i);
	}
	return (0);
}


// int	bui_echo(char **args)
// {
// 	int		i;
// 	int		newline;
// 	char	*processed_arg;

// 	if (args == NULL || args[0] == NULL)
// 	{
// 		fprintf(stderr, "\n");
// 		return (1);
// 	}
// 	i = check_n_flag(args, &newline);
// 	if (ft_strcmp(args[0], "-e") == 0)
// 	{
// 		i++;
// 		while (args[i])
// 		{
// 			processed_arg = process_escape_sequences(args[i]);
// 			if (processed_arg)
// 			{
// 				printf("%s\n", processed_arg);
// 				free(processed_arg);
// 			}
// 			if (args[i + 1])
// 				printf(" ");
// 			i++;
// 		}
// 		return (0);
// 	}
// 	if (args[1] && strcmp(args[0], "$$") == 0)
// 		handle_echo_with_pid(args[1]);
// 	else
// 	{
// 		while (args[i])
// 		{
// 			printf("%s", args[i]);
// 			if (args[i + 1])
// 				printf(" ");
// 			i++;
// 		}
// 	}
// 	if (newline)
// 		printf("\n");
// 	return (0);
// }

int	setup_pipes(t_command *current_command, int *pipe_fd, int *last_exit_status)
{
	if (current_command->next)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("Pipe creation failed");
			*last_exit_status = 1;
			return (-1);
		}
	}
	return (0);
}
