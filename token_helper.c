/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 15:20:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/13 15:55:32 by pwojnaro         ###   ########.fr       */
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
