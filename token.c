/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 14:55:57 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/10 17:56:30 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*init_token(char *value, t_token_type type, t_memories *memories)
{
	t_token	*new_token;

	new_token = (t_token *)malloc(sizeof(t_token));
	if (!new_token)
	{
		printf("Error: Failed to allocate memory for token.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, new_token);
	new_token->value = strdup(value);
	add_memory(memories, new_token->value);
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
	}
	else
	{
		current = *head;
		while (current->next)
		{
			current = current->next;
		}
		current->next = new_token;
	}
}

char	*get_quoted_token(char **input_ptr, t_env *environment)
{
	char	*start;
	char	*end;
	char	quote_char;
	char	*result;
	char	*var_name;
	char	*value;

	result = malloc(1);
	result[0] = '\0';
	quote_char = **input_ptr;
	start = *input_ptr + 1;
	end = start;
	while (*end && *end != quote_char)
	{
		if (*end == '$')
		{
			strncat(result, start, end - start);
			end++;
			start = end;
			while (isalnum(*end) || *end == '_')
				end++;
			var_name = strndup(start, end - start);
			value = get_env_value(var_name, environment);
			free(var_name);
			if (value)
			{
				result = realloc(result, strlen(result) + strlen(value) + 1);
				strcat(result, value);
				free(value);
			}
			start = end;
		}
		else
		{
			end++;
		}
	}
	strncat(result, start, end - start);
	if (*end == quote_char)
		*input_ptr = end + 1;
	else
		*input_ptr = end;
	return (result);
}

void	handle_special_characters(char **input, t_token **token_list,
			t_memories *memories, int *expect_filename, int *last_exit_status)
{
	if (**input == '|')
	{
		add_token(token_list, init_token("|", TOKEN_PIPE, memories));
		(*input)++;
	}
	else if (**input == '<')
	{
		add_token(token_list, init_token("<", TOKEN_INPUT_REDIRECT, memories));
		(*input)++;
		*expect_filename = 1;
	}
	else if (**input == '>')
	{
		if (*(*input + 1) == '>')
		{
			add_token(token_list, init_token(">>",
					TOKEN_APPEND_OUTPUT_REDIRECT, memories));
			(*input) += 2;
		}
		else
		{
			add_token(token_list, init_token(">",
					TOKEN_OUTPUT_REDIRECT, memories));
			(*input)++;
		}
		*expect_filename = 1;
	}
	if (*expect_filename && (**input == '\0' || **input == '|'
			|| **input == '<' || **input == '>'))
	{
		*last_exit_status = 2;
		return ;
	}
}

void tokenize_input(char *input, t_token **token_list, t_memories *memories, t_env *environment, int *last_exit_status)
{
    char *token;
    char *start;
    int expect_filename = 0;
    char *value;

    printf("Debug: Starting tokenization. Last exit status = %d\n", *last_exit_status);

    while (*input)
    {
        skip_spaces(&input);
        printf("Debug: After skipping spaces, input = '%s'\n", input);

        if (*input == '\0')
            break;

        if (*input == '$')
        {
            if (*(input + 1) == '?')
            {
                char exit_status_str[12];
                snprintf(exit_status_str, sizeof(exit_status_str), "%d", *last_exit_status);
                printf("Debug: Found $?, replacing with last_exit_status = %s\n", exit_status_str);
                add_token(token_list, init_token(exit_status_str, TOKEN_ARGUMENT, memories));
                input += 2;
                continue;
            }
            else
            {
                input++;
                start = input;
                while (isalnum(*input) || *input == '_')
                    input++;
                token = strndup(start, input - start);
                value = get_env_value(token, environment);
                printf("Debug: Found variable $%s, value = %s\n", token, value ? value : "(null)");
                add_token(token_list, init_token(value ? value : "", TOKEN_ARGUMENT, memories));
                free(token);
                if (value) free(value);
            }
        }
        else if (*input == '"' || *input == '\'')
        {
            token = get_quoted_token(&input, environment);
            printf("Debug: Found quoted token: %s\n", token);
            if (token)
            {
                add_token(token_list, init_token(token, expect_filename ? TOKEN_FILENAME : TOKEN_ARGUMENT, memories));
                free(token);
                expect_filename = 0;
            }
        }
        else
        {
            start = input;
            while (*input && !isspace(*input) && *input != '|' && *input != '<' && *input != '>')
                input++;
            if (input > start)
            {
                token = strndup(start, input - start);
                printf("Debug: Found regular token: %s\n", token);
                add_token(token_list, init_token(token, expect_filename ? TOKEN_FILENAME : TOKEN_COMMAND, memories));
                free(token);
                expect_filename = 0;
            }

            // Handle special characters and detect syntax errors
            handle_special_characters(&input, token_list, memories, &expect_filename, last_exit_status);
            if (*last_exit_status == 2)
            {
                printf("Debug: Syntax error detected in handle_special_characters, setting last_exit_status to 2\n");
            }
        }
    }

    printf("Debug: Tokenization complete. Last exit status = %d\n", *last_exit_status);
}
