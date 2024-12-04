/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 14:11:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/04 11:11:50 by pwojnaro         ###   ########.fr       */
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

void	append_to_buffer(char **buffer, size_t *length, size_t *capacity,
	const char *start, size_t segment_len, t_memories *memories)
{
	char	*new_buffer;

	if (*length + segment_len + 1 > *capacity)
	{
		while (*length + segment_len + 1 > *capacity)
			*capacity *= 2;
		new_buffer = realloc(*buffer, *capacity);
		if (!new_buffer)
		{
			perror("Memory allocation failed");
			exit(EXIT_FAILURE);
		}
		if (*buffer == NULL)
		{
			add_memory(memories, new_buffer);
		}
		*buffer = new_buffer;
	}
	memcpy(*buffer + *length, start, segment_len);
	*length += segment_len;
	(*buffer)[*length] = '\0';
}

char	*get_double_quoted_token(char **input_ptr, t_env *environment,
	t_memories *memories)
{
	char	*start;
	char	*end;
	char	*result;
	char	*var_name;
	char	*value;
	size_t	result_len;
	size_t	result_capacity;

	result_len = 0;
	result_capacity = 64;
	start = *input_ptr + 1;
	end = start;
	result = malloc(result_capacity);
	if (!result)
	{
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	result[0] = '\0';
	while (*end && *end != '"')
	{
		if (*end == '$')
		{
			if (end > start)
			{
				append_to_buffer(&result, &result_len, &result_capacity, start,
					end - start, memories);
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
				append_to_buffer(&result, &result_len, &result_capacity, value,
					strlen(value), memories);
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
		append_to_buffer(&result, &result_len, &result_capacity,
			start, end - start, memories);
	}
	if (*end == '"')
	{
		*input_ptr = end + 1;
	}
	else
	{
		*input_ptr = end;
	}
	return (result);
}
