/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_helper3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 16:46:19 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/22 17:05:15 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_all_redirections(t_token **current_token,
	t_command *current_command, t_memories *memories)
{
	int	append_mode;

	append_mode = 0;
	if ((*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT)
		append_mode = 1;
	handle_redirections(current_token, current_command, memories, append_mode);
}

void	add_argument_to_command(t_token *current_token,
	t_command *current_command, t_memories *memories, int *arg_count)
{
	current_command->args[*arg_count] = strdup(current_token->value);
	add_memory(memories, current_command->args[*arg_count]);
	(*arg_count)++;
}

void	parse_input_to_commands(t_token *token_list, t_command **command_list,
	t_memories *memories)
{
	t_command	*command;
	t_token		*token;
	int			arg_count;

	arg_count = 0;
	token = token_list;
	command = NULL;
	while (token)
	{
		if (command == NULL && token->type == TOKEN_COMMAND)
		{
			command = initialize_command(token,
					command_list, memories);
			arg_count = 1;
		}
		else if ((token->type == TOKEN_ARGUMENT || token->type == TOKEN_COMMAND)
			&& command)
			add_argument_to_command(token, command,
				memories, &arg_count);
		else
			process_special_tokens(&token, &command, memories, &arg_count);
		token = token->next;
	}
	if (command)
		command->args[arg_count] = NULL;
}
