/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_helper.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 15:20:33 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/08 13:16:18 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	skip_spaces(char **input)
{
	while (**input && isspace(**input))
		(*input)++;
}

char	*get_env_value(const char *name, t_env *environment)
{
	int	i;

	i = 0;
	while (i < environment->size)
	{
		if (strcmp(environment->pairs[i].key, name) == 0)
		{
			return (strdup(environment->pairs[i].value));
		}
		i++;
	}
	return (NULL);
}

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
