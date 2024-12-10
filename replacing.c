/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replacing.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:45:32 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 18:29:21 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	ft_strlen(const char *str)
{
	size_t	length;

	length = 0;
	while (str[length] != '\0')
	{
		length++;
	}
	return (length);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;
	size_t				i;

	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dest);
}

char	*str_concat(const char *key, const char *value, const char *delimiter,
	t_memories *memories)
{
	size_t	key_len;
	size_t	value_len;
	size_t	delimiter_len;
	char	*result;

	key_len = ft_strlen(key);
	value_len = ft_strlen(value);
	delimiter_len = ft_strlen(delimiter);
	result = malloc(key_len + value_len + delimiter_len + 1);
	if (!result)
		return (NULL);
	ft_memcpy(result, key, key_len);
	ft_memcpy(result + key_len, delimiter, delimiter_len);
	ft_memcpy(result + key_len + delimiter_len, value, value_len);
	result[key_len + delimiter_len + value_len] = '\0';
	add_memory(memories, result);
	return (result);
}

char	*ft_strndup(const char *s, size_t n, t_memories *memories)
{
	size_t	len;
	char	*new_str;
	size_t	i;

	len = 0;
	i = 0;
	while (s[len] != '\0' && len < n)
	{
		len++;
	}
	new_str = malloc(len + 1);
	if (!new_str)
	{
		perror("Memory allocation failed");
		return (NULL);
	}
	while (i < len)
	{
		new_str[i] = s[i];
		i++;
	}
	new_str[len] = '\0';
	if (memories)
		add_memory(memories, new_str);
	return (new_str);
}

char	*ft_strcat(char *dest, const char *src)
{
	char	*dest_ptr;

	dest_ptr = dest;
	while (*dest_ptr != '\0')
	{
		dest_ptr++;
	}
	while (*src != '\0')
	{
		*dest_ptr = *src;
		dest_ptr++;
		src++;
	}
	*dest_ptr = '\0';
	return (dest);
}
