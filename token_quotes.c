/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 14:11:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/11 00:30:13 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_single_quoted_token(char **input_ptr, t_memories *memories)
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
	return (ft_strndup(start, end - start, memories));
}

void	append_to_buffer(t_buffer_info *buf_info, const char *start,
		size_t segment_len)
{
	char	*new_buffer;

	if (*(buf_info->length) + segment_len + 1 > *(buf_info->capacity))
	{
		while (*(buf_info->length) + segment_len + 1 > *(buf_info->capacity))
			*(buf_info->capacity) *= 2;
		new_buffer = malloc(*(buf_info->capacity));
		if (!new_buffer)
		{
			perror("Memory allocation failed");
			exit(EXIT_FAILURE);
		}
		ft_memcpy(new_buffer, *(buf_info->buffer), *(buf_info->length));
		if (is_in_memories(buf_info->memories, *(buf_info->buffer)))
			remove_memory(buf_info->memories, *(buf_info->buffer));
		*(buf_info->buffer) = new_buffer;
		add_memory(buf_info->memories, new_buffer);
	}
	ft_memcpy(*(buf_info->buffer) + *(buf_info->length), start, segment_len);
	*(buf_info->length) += segment_len;
	(*(buf_info->buffer))[*buf_info->length] = '\0';
}

char	*get_env_value(const char *name, const t_env *environment,
	t_memories *memories)
{
	size_t	i;

	i = 0;
	while (i < environment->size)
	{
		if (ft_strcmp(environment->pairs[i].key, name) == 0)
		{
			return (ft_strndup(environment->pairs[i].value,
					ft_strlen(environment->pairs[i].value), memories));
		}
		i++;
	}
	return (NULL);
}
