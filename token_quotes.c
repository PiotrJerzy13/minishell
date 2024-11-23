/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 14:11:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:46:17 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

char	*get_quoted_token(char **input_ptr, t_env *environment)
{
	char	quote_char;

	quote_char = **input_ptr;
	if (quote_char == '\'')
		return (get_single_quoted_token(input_ptr));
	else if (quote_char == '"')
		return (get_double_quoted_token(input_ptr, environment));
	else
		return (NULL);
}

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

char	*get_double_quoted_token(char **input_ptr, t_env *environment)
{
	char	*start;
	char	*end;
	char	*result;

	start = *input_ptr + 1;
	end = start;
	result = malloc(1);
	if (!result)
		exit(EXIT_FAILURE);
	result[0] = '\0';
	while (*end && *end != '"')
	{
		if (*end == '$')
			handle_dollar(&end, &start, &result, environment);
		else
			process_until_special(&end, &start, &result, '"');
	}
	append_to_result(&result, start, end - start);
	if (*end == '"')
		*input_ptr = end + 1;
	else
		*input_ptr = end;
	return (result);
}
