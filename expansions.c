/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansions.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 12:07:21 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 18:31:30 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_digits(int number)
{
	int	count;

	count = 0;
	if (number == 0)
		return (1);
	while (number > 0)
	{
		count++;
		number /= 10;
	}
	return (count);
}

void	reverse_buffer(char *buffer, int length)
{
	int		start;
	int		end;
	char	temp;

	start = 0;
	end = length - 1;
	while (start < end)
	{
		temp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = temp;
		start++;
		end--;
	}
}

void	convert_positive_int_to_string(int number, char *buffer)
{
	int	index;

	index = 0;
	if (number == 0)
	{
		buffer[index++] = '0';
		buffer[index] = '\0';
		return ;
	}
	while (number > 0)
	{
		buffer[index++] = '0' + (number % 10);
		number /= 10;
	}
	buffer[index] = '\0';
	reverse_buffer(buffer, index);
}

void	handle_exit_status_expansion(char **input, t_token_context *context)
{
	char	exit_status_str[12];

	int_to_string(*(context->last_exit_status), exit_status_str);
	add_token(context->token_list, init_token(exit_status_str,
			TOKEN_ARGUMENT, context->memories));
	*input += 2;
}

void	process_variable_expansion(char **input, t_token_context *context)
{
	if (*(*input + 1) == '?')
	{
		handle_exit_status_expansion(input, context);
	}
	else
	{
		handle_variable_expansion(input, context);
	}
}
