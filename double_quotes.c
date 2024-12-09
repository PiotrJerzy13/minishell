/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_quotes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 15:33:14 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 15:45:48 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_buffer_info	initialize_buffer_info(t_memories *memories)
{
	t_buffer_info	buf_info;

	buf_info.buffer = malloc(sizeof(char *));
	if (!buf_info.buffer)
	{
		perror("Memory allocation failed for buffer pointer");
		exit(EXIT_FAILURE);
	}
	*buf_info.buffer = malloc(64);
	if (!*buf_info.buffer)
	{
		perror("Memory allocation failed for buffer");
		exit(EXIT_FAILURE);
	}
	buf_info.length = malloc(sizeof(size_t));
	buf_info.capacity = malloc(sizeof(size_t));
	if (!buf_info.length || !buf_info.capacity)
	{
		exit(EXIT_FAILURE);
	}
	*buf_info.length = 0;
	*buf_info.capacity = 64;
	**buf_info.buffer = '\0';
	buf_info.memories = memories;
	return (buf_info);
}

void	process_variable(char **start, char **end, t_buffer_info *buf_info,
			t_env *environment)
{
	char	*var_name;
	char	*value;

	(*end)++;
	*start = *end;
	while (isalnum(**end) || **end == '_')
		(*end)++;
	var_name = strndup(*start, *end - *start);
	if (!var_name)
	{
		perror("Memory allocation failed for variable name");
		exit(EXIT_FAILURE);
	}
	value = get_env_value(var_name, environment);
	free(var_name);
	if (value)
	{
		append_to_buffer(buf_info, value, strlen(value));
		free(value);
	}
	*start = *end;
}

char	*get_double_quoted_token(char **input_ptr, t_env *environment,
			t_memories *memories)
{
	char			*start;
	char			*end;
	t_buffer_info	buf_info;

	buf_info = initialize_buffer_info(memories);
	start = *input_ptr + 1;
	end = start;
	while (*end && *end != '"')
	{
		if (*end == '$')
		{
			if (end > start)
				append_to_buffer(&buf_info, start, end - start);
			process_variable(&start, &end, &buf_info, environment);
		}
		else
			end++;
	}
	if (end > start)
		append_to_buffer(&buf_info, start, end - start);
	if (*end == '"')
		*input_ptr = end + 1;
	else
		*input_ptr = end;
	return (*buf_info.buffer);
}
