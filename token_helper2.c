/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 12:36:26 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/19 17:48:09 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	append_to_result(char **result, const char *start, size_t length)
{
	*result = realloc(*result, strlen(*result) + length + 1);
	if (!*result)
		exit(EXIT_FAILURE);
	strncat(*result, start, length);
}

void	handle_variable_expansion1(char **result, char **end_ptr,
	t_env *environment)
{
	char	*end;
	char	*start;
	char	*var_name;
	char	*value;

	end = *end_ptr;
	start = end;
	while (isalnum(*end) || *end == '_')
		end++;
	var_name = strndup(start, end - start);
	if (!var_name)
		exit(EXIT_FAILURE);
	value = get_env_value(var_name, environment);
	free(var_name);
	if (value)
	{
		append_to_result(result, value, strlen(value));
		free(value);
	}
	*end_ptr = end;
}

void	process_until_special(char **end_ptr, char **start_ptr, char **result,
	char stop_char)
{
	char	*end;
	char	*start;

	end = *end_ptr;
	start = *start_ptr;
	while (*end && *end != stop_char && *end != '$')
		end++;
	append_to_result(result, start, end - start);
	*end_ptr = end;
	*start_ptr = end;
}

void	handle_dollar(char **end, char **start, char **result,
	t_env *environment)
{
	process_until_special(end, start, result, '$');
	handle_variable_expansion1(result, end, environment);
	*start = *end;
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
