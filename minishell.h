/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:37:13 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/04 17:50:03 by pwojnaro         ###   ########.fr       */
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
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/stat.h>

#define INITIAL_CAPACITY 128
#define MAX_ARGS 128  // Adjust the number based on your requirements
#define SUCCESS 0
#define FAILURE 1

typedef struct s_heredoc_node
{
	char					*line;
	struct s_heredoc_node	*next;
}	t_heredoc_node;
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
	size_t		size;
	size_t		capacity;
	int			cd_used_flag;
	t_memories	*memories;
	int			path_unset;
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
	int					append_mode;
	int					append_output;
	t_heredoc_node		*heredoc_list;
	struct s_command	*next;
}	t_command;

typedef enum e_token_type
{
	TOKEN_COMMAND,
	TOKEN_ARGUMENT,
	TOKEN_PIPE,
	TOKEN_INPUT_REDIRECT,
	TOKEN_OUTPUT_REDIRECT,
	TOKEN_APPEND_OUTPUT_REDIRECT,
	TOKEN_HEREDOC,
	TOKEN_DELIMITER,
	TOKEN_FILENAME
}	t_token_type;
typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef struct s_exec_context
{
	int		in_fd;
	int		*pipefd;
	int		*last_exit_status;
	t_env	*environment;
	int		saved_stdin;
	int		saved_stdout;
}	t_exec_context;

typedef struct s_token_context
{
	t_token		**token_list;
	t_memories	*memories;
	t_env		*environment;
	int			*last_exit_status;
	int			expect_filename;
}	t_token_context;

typedef struct s_token_info
{
	char	*single_token;
	char	*double_token;
	int		type_single;
	int		type_double;
}	t_token_info;

typedef struct s_shell_state
{
	t_command	*command_list;
	t_token		*token_list;
	t_memories	memories;
	t_env		environment;
	int			last_exit_status;
	char		*input;
}	t_shell_state;

typedef struct s_command_context
{
	t_token		**token_list;
	t_command	**command_list;
	t_memories	*memories;
	t_env		*environment;
	int			*last_exit_status;
}	t_command_context;

void			init_memories(t_memories *memories, t_env *environment,
					int env_capacity);
void			add_memory(t_memories *memories, void *ptr);
void			free_all_memories(t_memories *memories);
int				parse_input_to_commands(t_token *token_list,
					t_command **command_list, t_memories *memories);
void			tokenize_input(char *input, t_token_context *context);
void			skip_spaces(char **input);
void			copy_environment_to_struct(char **env, t_env *environment,
					t_memories *memories);
void			add_or_update_env_var(t_env *env, const char *key,
					const char *value, t_memories *memories);
void			print_env(t_env *env);
void			export_env_var(t_env *environment, char *input,
					t_memories *memories);
int				bui_echo(char **args);
int				bui_cd(char **args, t_env *env, t_memories *memories);
int				bui_exit(char **args);
void			execute_commands(t_command *command_list, int *last_exit_status,
					t_env *environment);
int				initialize_shell(void);
char			*get_env_value(const char *name, t_env *environment);
char			*ft_strtrim(char *str, const char *set);
char			*get_next_line(int fd);
int				handle_builtin(t_command *command, t_env *environment,
					t_memories *memories, int *exit_st);
void			free_env_array(char **env_array);
char			**env_to_char_array(t_env *environment);
char			*find_executable_path(const char *command);
void			unset_env_var(t_env *env, const char *key);
void			add_token(t_token **head, t_token *new_token);
int				collect_heredoc_input(const char *delimiter,
					t_heredoc_node **heredoc_list);
char			*get_double_quoted_token(char **input_ptr, t_env *environment,
					t_memories *memories);
char			*get_single_quoted_token(char **input_ptr);
int				clear_output_redirect(const char *output_redirect,
					int *last_exit_status);
void			restore_redirections(int saved_stdin, int saved_stdout);
void			handle_unset(t_command *command, t_env *environment);
void			execute_commands(t_command *command_list, int *last_exit,
					t_env *environment);
void			handle_special_characters(char **input,
					t_token_context *context);
t_token			*init_token(char *value, t_token_type type,
					t_memories *memories);
void			process_variable_expansion(char **input,
					t_token_context *context);
void			process_general_token(char **input, t_token_context *context);
void			handle_variable_expansion(char **input,
					t_token_context *context);
void			process_quoted_token(char **input, t_token_context *context);
t_command		*initialize_command(t_token *token,
					t_command **command_list, t_memories *memories);
int				handle_all_redirections(t_token **current_token,
					t_command *current_command, t_memories *memories);
int				handle_redirections(t_token **current_token,
					t_command *current_command, t_memories *memories);
int				process_special_tokens(t_token **current_token,
					t_command **current_command, t_memories *memories,
					int *arg_count);
void			init_shell_state(t_shell_state *state);
void			process_commands(char *input, t_command_context *context);
char			*get_user_input(void);
int				initialize_shell_environment(t_memories *memories,
					t_env *environment, char **env);
void			add_argument_to_command(t_token *current_token,
					t_command *current_command, t_memories *memories,
					int *arg_count);
void			handle_pipe(t_command **current_command, int *arg_count);
int				handle_heredoc(t_token **current_token,
					t_command *current_command);
void			handle_token_creation(char **input, t_token_context *context,
					t_token_info *info);
void			append_to_result(char **result, const char *start,
					size_t length);
int				handle_input_redirection(const char *input_redirect,
					int *saved_stdin, int	*last_exit_status);
int				handle_output_redirection(const char *output_redirect,
					int append_mode, int *saved_stdout, int *last_exit_status);
t_token_context	init_token_context(t_command_context *context);
t_command		*create_new_command(t_memories *memories);
int				validate_export_argument(const char *arg);