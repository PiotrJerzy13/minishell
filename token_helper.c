/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 15:20:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 12:29:50 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	skip_spaces(char **input)
{
	while (**input && isspace(**input))
		(*input)++;
}

char	*get_next_line(int fd)
{
	char	*line;
	int		i;
	char	ch;

	line = malloc(1024);
	i = 0;
	if (!line)
		return (NULL);
	while (read(fd, &ch, 1) > 0 && ch != '\n')
	{
		line[i++] = ch;
		if (i >= 1023)
			break ;
	}
	line[i] = '\0';
	if (i == 0 && ch != '\n')
	{
		free(line);
		return (NULL);
	}
	else
	{
		return (line);
	}
}
