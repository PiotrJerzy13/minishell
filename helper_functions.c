/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 13:20:23 by kkaratsi          #+#    #+#             */
/*   Updated: 2024/12/10 14:15:14 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_atoi(const char *str)
{
	int	i;
	int	sign;
	int	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\v' || str[i] == '\f' || str[i] == '\r')
		i++;
	if (str[i] == '-')
		sign = -1;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

int	ft_strcmp(const char *first_str, const char *second_str)
{
	while (*first_str && *second_str)
	{
		if (*first_str != *second_str)
			return (*first_str - *second_str);
		first_str++;
		second_str++;
	}
	return (*first_str - *second_str);
}

char	*ft_strchr(const char *s, int c)
{
	char	p;

	p = c;
	while (*s != '\0')
	{
		if (*s == p)
			return ((char *)s);
		s++;
	}
	if (p == '\0')
		return ((char *)s);
	return (NULL);
}

char	*ft_strtok(char *str, const char *delim)
{
	static char	*current_pos;
	char		*start;

	current_pos = NULL;
	if (str != NULL)
		current_pos = str;
	if (current_pos == NULL)
		return (NULL);
	while (*current_pos != '\0' && ft_strchr(delim, *current_pos) != NULL)
		current_pos++;
	if (*current_pos == '\0')
		return (NULL);
	start = current_pos;
	while (*current_pos != '\0' && ft_strchr(delim, *current_pos) == NULL)
		current_pos++;
	if (*current_pos != '\0')
	{
		*current_pos = '\0';
		current_pos++;
	}
	return (start);
}
