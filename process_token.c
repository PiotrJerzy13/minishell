/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:09:48 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/04 13:20:58 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**
 * process_general_token - Processes general tokens (commands 
 * or filenames).
 *
 * This function parses a general token from the input string, 
 * which is any sequence
 * of non-whitespace characters not including special shell 
 * characters (`|`, `<`, `>`).
 * It identifies whether the token represents a command or 
 * a filename based on the 
 * context and adds it to the token list.
 */
void	process_general_token(char **input, t_token_context *context)
{
	char	*start;
	char	*token;

	start = *input;
	while (**input && !isspace(**input) && **input != '|'
		&& **input != '<' && **input != '>')
		(*input)++;
	if (*input > start)
	{
		token = strndup(start, *input - start);
		if (context->expect_filename)
		{
			add_token(context->token_list, init_token(token,
					TOKEN_FILENAME, context->memories));
		}
		else
		{
			add_token(context->token_list, init_token(token,
					TOKEN_COMMAND, context->memories));
		}
		free(token);
		context->expect_filename = 0;
	}
	handle_special_characters(input, context);
}

/**
 * process_quoted_token - Processes tokens enclosed in quotes.
 *
 * This function handles quoted tokens (e.g., `"some text"` or
 *  `'some text'`) by extracting
 * the quoted content, performing any necessary expansions,
 *  and adding it to the token list
 * as either a `TOKEN_FILENAME` or a `TOKEN_ARGUMENT`, 
 * based on the context.
 */

void	process_quoted_token(char **input, t_token_context *context)
{
	char	*token;

	token = NULL;
	if (**input == '"')
	{
		token = get_double_quoted_token(input, context->environment,
				context->memories);
	}
	else if (**input == '\'')
	{
		token = get_single_quoted_token(input);
	}
	if (token)
	{
		if (context->expect_filename)
		{
			add_token(context->token_list, init_token(token, TOKEN_FILENAME,
					context->memories));
		}
		else
		{
			add_token(context->token_list, init_token(token, TOKEN_ARGUMENT,
					context->memories));
		}
		free(token);
	}
}

/**
 * handle_redirects - Handles input and output redirection tokens.
 *
 * This function processes redirection operators 
 * (`<`, `>`, `<<`, `>>`) and adds the
 * appropriate redirection tokens to the token list. 
 * It distinguishes between single and
 * double redirection operators (e.g., `>` vs. `>>`).
 */
void	handle_redirects(char **input, t_token_context *context,
			char redirect_char)
{
	t_token_info	info;

	if (redirect_char == '<')
	{
		info.single_token = "<";
		info.double_token = "<<";
		info.type_single = TOKEN_INPUT_REDIRECT;
		info.type_double = TOKEN_HEREDOC;
	}
	else
	{
		info.single_token = ">";
		info.double_token = ">>";
		info.type_single = TOKEN_OUTPUT_REDIRECT;
		info.type_double = TOKEN_APPEND_OUTPUT_REDIRECT;
	}
	handle_token_creation(input, context, &info);
}

/**
 * handle_special_characters - Processes special shell characters.
 *
 * This function identifies and processes special 
 * characters in the input, such as `|` (pipe)
 * and redirection operators (`<`, `>`). It adds 
 * the appropriate tokens to the token list and
 * handles context-specific behavior for filenames and errors.
 */
void	handle_special_characters(char **input, t_token_context *context)
{
	if (**input == '|')
	{
		add_token(context->token_list, init_token("|",
				TOKEN_PIPE, context->memories));
		(*input)++;
	}
	else if (**input == '<' || **input == '>')
		handle_redirects(input, context, **input);
	if (context->expect_filename && (**input == '\0' || **input == '|'
			|| **input == '<' || **input == '>'))
		*(context->last_exit_status) = 258;
}

int	process_special_tokens(t_token **current_token, t_command **current_command,
				t_memories *memories, int *arg_count)
{
	if ((*current_token)->type == TOKEN_HEREDOC && *current_command)
	{
		if (handle_heredoc(current_token, *current_command) == -1)
		{
			fprintf(stderr, "Error: Failed to handle heredoc.\n");
			return (-1);
		}
	}
	else if (((*current_token)->type == TOKEN_OUTPUT_REDIRECT
			|| (*current_token)->type == TOKEN_APPEND_OUTPUT_REDIRECT
			|| (*current_token)->type == TOKEN_INPUT_REDIRECT)
		&& *current_command)
	{
		if (handle_all_redirections(current_token, *current_command,
				memories) == -1)
		{
			fprintf(stderr, "Error: Failed to handle redirection.\n");
			return (-1);
		}
	}
	else if ((*current_token)->type == TOKEN_PIPE)
	{
		if (!*current_command)
		{
			fprintf(stderr, "Error: Pipe '|' without preceding command.\n");
			return (-1);
		}
		handle_pipe(current_command, arg_count);
		if (!(*current_token)->next || (*current_token)->next->type
			!= TOKEN_COMMAND)
		{
			fprintf(stderr, "Error: Pipe '|' without following command.\n");
			return (-1);
		}
	}
	else
	{
		fprintf(stderr, "Unrecognized special token: %s\n",
			(*current_token)->value);
		return (-1);
	}
	return (0);
}
