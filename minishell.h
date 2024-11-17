/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:37:13 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/17 15:54:10 by pwojnaro         ###   ########.fr       */
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
	TOKEN_FILENAME
}	t_token_type;
typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

void			init_memories(t_memories *memories, t_env *environment,
					int env_capacity);
void			add_memory(t_memories *memories, void *ptr);
void			free_all_memories(t_memories *memories);
void			parse_input_to_commands(t_token *token_list,
					t_command **command_list,
					t_memories *memories);
t_command		*init_command_node(t_memories *memories);
void			tokenize_input(char *input, t_token **token_list,
					t_memories *memories, t_env *environment,
					int *last_exit_status);

void			skip_spaces(char **input);
void			copy_environment_to_struct(char **env, t_env *environment,
					t_memories *memories);
void			add_or_update_env_var(t_env *env, const char *key,
					const char *value, t_memories *memories);
void			print_env(t_env *env);
void			export_env_var(t_env *environment, char *input,
					t_memories *memories);
int				bui_echo(char **args);
int				bui_cd(char **args);
int				bui_pwd(void);
int				bui_exit(char **args);
void			execute_commands(t_command *command_list, int *last_exit_status,
					t_env *environment);
int				initialize_shell(void);
char			*get_env_value(const char *name, t_env *environment);
char			*ft_strtrim(char *str, const char *set);
char			*get_next_line(int fd);
int				handle_builtin(t_command *command, t_env *environment,
					t_memories *memories, int *last_exit_status);
void			free_env_array(char **env_array);
char			**env_to_char_array(t_env *environment);
int				is_same_file(const char *file1, const char *file2);
char			*find_executable_path(const char *command);
void			unset_env_var(t_env *env, const char *key);
void			add_token(t_token **head, t_token *new_token);
void			free_heredoc_list(t_heredoc_node *head);
int				collect_heredoc_input(const char *delimiter, t_heredoc_node **heredoc_list);
