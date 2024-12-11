/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 14:50:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/11 00:44:27 by pwojnaro         ###   ########.fr       */
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

char	*process_escape_sequences(const char *input, t_memories *memories)
{
	size_t	len;
	char	*result;
	char	*dst;

	len = ft_strlen(input);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	add_memory(memories, result);
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

void	process_echo_arguments(char **args, int start_index,
		int escape_sequences, t_memories *memories)
{
	const char	*processed_arg;
	int			i;

	i = start_index;
	while (args[i])
	{
		if (escape_sequences)
			processed_arg = process_escape_sequences(args[i], memories);
		else
			processed_arg = args[i];
		if (processed_arg)
			printf("%s", processed_arg);
		if (args[i + 1])
			printf(" ");
		i++;
	}
}

int	bui_echo(char **args, t_memories *memories)
{
	int	i;
	int	newline;

	if (!args || !args[0])
	{
		printf("\n");
		return (1);
	}
	i = check_n_flag(args, &newline);
	if (ft_strcmp(args[0], "-e") == 0)
	{
		process_echo_arguments(args, i + 1, 1, memories);
		printf("\n");
		return (0);
	}
	if (args[1] && ft_strcmp(args[0], "$$") == 0)
		handle_echo_with_pid(args[1]);
	else
		process_echo_arguments(args, i, 0, memories);
	if (newline)
		printf("\n");
	return (0);
}
