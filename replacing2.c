/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replacing2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 18:28:51 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/11 00:16:47 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strcpy(char *dest, const char *src)
{
	char	*dest_ptr;

	dest_ptr = dest;
	while (*src != '\0')
	{
		*dest_ptr = *src;
		dest_ptr++;
		src++;
	}
	*dest_ptr = '\0';
	return (dest);
}

void	handle_negative_number(char *buffer, int positive_number)
{
	int	length;
	int	i;

	length = count_digits(positive_number);
	i = length;
	while (i > 0)
	{
		buffer[i] = buffer[i - 1];
		i--;
	}
	buffer[0] = '-';
	buffer[length + 1] = '\0';
}

void	int_to_string(int number, char *buffer)
{
	int	is_negative;
	int	positive_number;

	is_negative = 0;
	if (number < 0)
	{
		is_negative = 1;
		positive_number = -number;
	}
	else
	{
		positive_number = number;
	}
	convert_positive_int_to_string(positive_number, buffer);
	if (is_negative)
	{
		handle_negative_number(buffer, positive_number);
	}
}

void	handle_variable_expansion(char **input, t_token_context *context)
{
	char	*start;
	char	*token;
	char	*value;

	(*input)++;
	start = *input;
	while (ft_isalnum(**input) || **input == '_')
		(*input)++;
	token = ft_strndup(start, *input - start, context->memories);
	value = get_env_value(token, context->environment, context->memories);
	if (value != NULL)
	{
		add_token(context->token_list, init_token(value,
				TOKEN_ARGUMENT, context->memories));
	}
	else
	{
		add_token(context->token_list, init_token("",
				TOKEN_ARGUMENT, context->memories));
	}
}

int	setup_pipes(t_command *current_command, int *pipe_fd, int *last_exit_status)
{
	if (current_command->next)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("Pipe creation failed");
			*last_exit_status = 1;
			return (-1);
		}
	}
	return (0);
}
