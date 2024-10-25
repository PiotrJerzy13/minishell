/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 15:20:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/25 13:30:54 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	skip_spaces(char **input)
{
	while (**input && isspace(**input))
		(*input)++;
}

char	*handle_quoted_string(char **input)
{
	char	*start;
	char	*end;

	start = *input + 1;
	end = start;
	while (*end && *end != '"')
		end++;
	if (*end == '"')
	{
		*end = '\0';
		*input = end + 1;
		return (strdup(start));
	}
	printf("Error: Unmatched quote in input.\n");
	return (NULL);
}

int	bitwise_compare(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] && s2[i])
	{
		if ((s1[i] ^ s2[i]) != 0)
			return (0);
		i++;
	}
	if (i < n && (s1[i] != '\0' || s2[i] != '\0'))
		return (0);
	return (1);
}

t_builtin_ptr	get_builtin(const t_prompt *prompt)
{
	if (bitwise_compare(prompt->cmd, "echo", 4))
		return (bui_echo);
	else if (bitwise_compare(prompt->cmd, "cd", 2))
		return (bui_cd);
	else if (bitwise_compare(prompt->cmd, "pwd", 3))
		return (bui_pwd);
	else if (bitwise_compare(prompt->cmd, "export", 6))
		return (bui_export);
	else if (bitwise_compare(prompt->cmd, "unset", 5))
		return (bui_unset);
	else if (bitwise_compare(prompt->cmd, "env", 3))
		return (bui_env);
	else if (bitwise_compare(prompt->cmd, "exit", 4))
		return (bui_exit);
	return (NULL);
}
