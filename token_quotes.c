/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 14:11:59 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/24 16:02:52 by pwojnaro         ###   ########.fr       */
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
	char	*var_name;
	char	*value;

	start = *input_ptr + 1;
	end = start;
	result = malloc(1);
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
			append_to_result(&result, start, end - start);
			end++;
			start = end;
			while (isalnum(*end) || *end == '_')
			{
				end++;
			}
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
				append_to_result(&result, value, strlen(value));
				free(value);
			}
			start = end;
		}
		else
		{
			while (*end && *end != '"' && *end != '$')
			{
				end++;
			}
		}
	}
	append_to_result(&result, start, end - start);
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
