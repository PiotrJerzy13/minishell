/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 15:20:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/25 14:11:50 by pwojnaro         ###   ########.fr       */
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

t_builtin_ptr	get_builtin(const char *command)
{
	if (strcmp(command, "echo") == 0)
		return (bui_echo);
	else if (strcmp(command, "cd") == 0)
		return (bui_cd);
	else if (strcmp(command, "pwd") == 0)
		return (bui_pwd);
	else if (strcmp(command, "export") == 0)
		return (bui_export);
	else if (strcmp(command, "unset") == 0)
		return (bui_unset);
	else if (strcmp(command, "env") == 0)
		return (bui_env);
	else if (strcmp(command, "exit") == 0)
		return (bui_exit);
	return (NULL);
}
