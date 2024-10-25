/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:37:13 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/23 13:13:13 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#define INITIAL_CAPACITY 128

typedef struct s_memories
{
	void	**allocations;
	int		size;
	int		capacity;
}	t_memories;

typedef struct s_key_value
{
	char	*key;
	char	*value;
}	t_key_value;

typedef struct s_env
{
	t_key_value	*pairs;
	int			size;
	int			capacity;
	t_memories	*memories;
}	t_env;

typedef struct s_key_value_store
{
	t_key_value	*pairs;
	int			size;
	int			capacity;
}	t_key_value_store;

typedef struct s_command
{
	char				*command;
	char				**args;
	int					is_pipe;
	char				*input_redirect;
	char				*output_redirect;
	struct s_command	*next;
}	t_command;

typedef enum e_token_type
{
	TOKEN_COMMAND,
	TOKEN_ARGUMENT,
	TOKEN_PIPE,
	TOKEN_INPUT_REDIRECT,
	TOKEN_OUTPUT_REDIRECT,
	TOKEN_APPEND_OUTPUT_REDIRECT
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

void	init_memories(t_memories *memories);
void	add_memory(t_memories *memories, void *ptr);
void	free_all_memories(t_memories *memories);
void	init_key_value_store(t_key_value_store *store, int initial_capacity,
			t_memories *memories);
void	parse_input_to_key_value(t_key_value_store *store,
			t_memories *memories);
void	print_key_value_store(t_key_value_store *store);
char	*allocate_user_input(t_memories *memories);

void	parse_input_to_commands(t_command **command_list, t_memories *memories);
void	add_command_node(t_command **head, t_command *new_command);
void	free_command_list(t_command *head);
void	tokenize_input(char *input, t_token **token_list);
void	skip_spaces(char **input);
void	handle_special_characters(char **input, t_token **token_list);
t_token	*init_token(char *value, t_token_type type);
void	add_token(t_token **token_list, t_token *new_token);
char	*handle_quoted_string(char **input);

void	init_env(t_env *env, int initial_capacity, t_memories *memories);
void	copy_environment_to_struct(char **env, t_env *env_struct,
			t_memories *memories);
void	add_or_update_env_var(t_env *env, const char *key,
			const char *value, t_memories *memories);
void	unset_env_var(t_env *env, const char *key);
void	print_env(t_env *env);
void	free_env(t_env *env);
void	export_env_var(t_env *env, char *input);
int		bui_echo(void);
int		bui_cd(void);
int		bui_pwd(void);
int		bui_export(void);
int		bui_unset(void);
int		bui_env(void);
int		bui_exit(void);