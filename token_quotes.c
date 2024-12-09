/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 14:11:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/09 12:57:47 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_single_quoted_token(char **input_ptr)
{
	char	*start;
	char	*end;

	start = *input_ptr + 1;
	end = start;
	while (*end && *end != '\'')
		end++;
	if (*end == '\'')
		*input_ptr = end + 1;
	else
		*input_ptr = end;
	return (strndup(start, end - start));
}

void	append_to_buffer(t_buffer_info *buf_info, const char *start,
	size_t segment_len)
{
	char	*new_buffer;

	if (*(buf_info->length) + segment_len + 1 > *(buf_info->capacity))
	{
		while (*(buf_info->length) + segment_len + 1 > *(buf_info->capacity))
			*(buf_info->capacity) *= 2;
		new_buffer = realloc(*(buf_info->buffer), *(buf_info->capacity));
		if (!new_buffer)
		{
			perror("Memory allocation failed");
			exit(EXIT_FAILURE);
		}
		if (*(buf_info->buffer) == NULL)
		{
			add_memory(buf_info->memories, new_buffer);
		}
		*(buf_info->buffer) = new_buffer;
	}
	memcpy(*(buf_info->buffer) + *(buf_info->length), start, segment_len);
	*(buf_info->length) += segment_len;
	(*(buf_info->buffer))[*buf_info->length] = '\0';
}

char	*get_double_quoted_token(char **input_ptr, t_env *environment,
		t_memories *memories)
{
	char			*start;
	char			*end;
	char			*var_name;
	char			*value;
	t_buffer_info	buf_info;

	buf_info.buffer = malloc(sizeof(char *));
	if (!buf_info.buffer)
	{
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	*buf_info.buffer = malloc(64);
	if (!*buf_info.buffer)
	{
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	buf_info.length = malloc(sizeof(size_t));
	buf_info.capacity = malloc(sizeof(size_t));
	if (!buf_info.length || !buf_info.capacity)
	{
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	*buf_info.length = 0;
	*buf_info.capacity = 64;
	buf_info.memories = memories;
	**buf_info.buffer = '\0';
	start = *input_ptr + 1;
	end = start;
	while (*end && *end != '"')
	{
		if (*end == '$')
		{
			if (end > start)
			{
				append_to_buffer(&buf_info, start, end - start);
			}
			end++;
			start = end;
			while (isalnum(*end) || *end == '_')
				end++;
			var_name = strndup(start, end - start);
			if (!var_name)
			{
				perror("Memory allocation failed for variable name");
				exit(EXIT_FAILURE);
			}
			value = get_env_value(var_name, environment);
			free(var_name);
			if (value)
			{
				append_to_buffer(&buf_info, value, strlen(value));
				free(value);
			}
			start = end;
		}
		else
		{
			end++;
		}
	}
	if (end > start)
	{
		append_to_buffer(&buf_info, start, end - start);
	}
	if (*end == '"')
	{
		*input_ptr = end + 1;
	}
	else
	{
		*input_ptr = end;
	}
	return (*buf_info.buffer);
}
