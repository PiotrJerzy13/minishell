/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 15:11:23 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 13:36:20 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// int	process_input_redirection(t_token **current_token,
// 		t_command *current_command, t_memories *memories)
// {
// 	char	*redirect;

// 	*current_token = (*current_token)->next;
// 	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
// 	{
// 		redirect = strdup((*current_token)->value);
// 		if (!redirect)
// 		{
// 			printf("Error: Failed to allocate memory.\n");
// 			return (-1);
// 		}
// 		add_memory(memories, redirect);
// 		current_command->input_redirect = redirect;
// 		return (0);
// 	}
// 	printf("Error: Missing filename for input redirection.\n");
// 	return (-1);
// }
int	process_input_redirection(t_token **current_token,
		t_command *current_command, t_memories *memories)
{
	char	*redirect;

	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		redirect = ft_strndup((*current_token)->value,
				ft_strlen((*current_token)->value), memories);
		if (!redirect)
		{
			printf("Error: Failed to allocate memory.\n");
			return (-1);
		}
		current_command->input_redirect = redirect;
		return (0);
	}
	printf("Error: Missing filename for input redirection.\n");
	return (-1);
}

int	process_output_redirection(t_token **current_token,
		t_command *current_command, t_memories *memories)
{
	char	*redirect;

	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		redirect = ft_strndup((*current_token)->value,
				ft_strlen((*current_token)->value), memories);
		if (!redirect)
		{
			printf("Error: Failed to allocate memory.\n");
			return (-1);
		}
		current_command->output_redirect = redirect;
		current_command->append_mode = 0;
		return (0);
	}
	printf("Error: Missing filename for output redirection.\n");
	return (-1);
}

int	process_append_redirection(t_token **current_token,
		t_command *current_command, t_memories *memories)
{
	char	*redirect;

	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		redirect = ft_strndup((*current_token)->value,
				ft_strlen((*current_token)->value), memories);
		if (!redirect)
		{
			printf("Error: Failed to allocate memory.\n");
			return (-1);
		}
		current_command->output_redirect = redirect;
		current_command->append_mode = 1;
		return (0);
	}
	printf("Error: Missing filename for append redirection.\n");
	return (-1);
}

int	handle_redirections(t_token **current_token, t_command *current_command,
	t_memories *memories)
{
	if (!current_command)
	{
		printf("Error: No current command to handle redirections.\n");
		return (-1);
	}
	if ((*current_token)->type == TOKEN_INPUT_REDIRECT)
	{
		return (process_input_redirection(current_token,
				current_command, memories));
	}
	else if ((*current_token)->type == TOKEN_OUTPUT_REDIRECT)
	{
		return (process_output_redirection(current_token,
				current_command, memories));
	}
	else if ((*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT)
	{
		return (process_append_redirection(current_token,
				current_command, memories));
	}
	else
	{
		return (-1);
	}
}
