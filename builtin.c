/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:20:56 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/09 12:43:57 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// The chdir function in C is used to change the current working directory
// of the calling process.
// Error handling not implemented using perror() function.
// Using atoi() and other build function for example strlen strcmp.
// The getcwd function in C is used to obtain the current working directory.
// Do we want to use whole Libft functions or just some of them?

// int	bui_cd(char **args)
// {
// 	const char	*path;

// 	if (args[0] == NULL)
// 	{
// 		path = getenv("HOME");
// 		if (path == NULL)
// 		{
// 			fprintf(stderr, "cd: HOME not set\n");
// 			return (1);
// 		}
// 	}
// 	else
// 	{
// 		path = args[0];
// 	}
// 	if (chdir(path) != 0)
// 	{
// 		perror("cd");
// 		return (1);
// 	}
// 	return (0);
// }

/* The set_env_value function is used to set the value of an environment 
variable. */

int	set_env_value(const char *name, const char *value, t_env *env)
{
	t_memories	*memories;
	int			i;

	i = 0;
	memories = env->memories;
	while (i < env->size)
	{
		if (strcmp(env->pairs[i].key, name) == 0)
		{
			free(env->pairs[i].value);
			env->pairs[i].value = strdup(value);
			if (env->pairs[i].value == NULL)
			{
				fprintf(stderr, "set_env_value: strdup failed\n");
				return (FAILURE);
			}
			return (SUCCESS);
		}
		i++;
	}
	add_or_update_env_var(env, name, value, memories);
	return (SUCCESS);
}

// int	bui_cd(char **args, t_env *env)
// {
// 	const char	*path;

// 	if (args[0] == NULL)
// 	{
// 		path = get_env_value("HOME", env);
// 		if (path == NULL)
// 		{
// 			fprintf(stderr, "cd: HOME not set\n");
// 			return (1);
// 		}
// 	}
// 	else
// 	{
// 		path = args[0];
// 	}
// 	if (chdir(path) != 0)
// 	{
// 		perror("cd");
// 		return (1);
// 	}
// 	set_env_value("OLDPWD", get_env_value("PWD", env), env);
// 	set_env_value("PWD", getcwd(NULL, 0), env);
// 	return (0);
// }

int	bui_cd(char **args, t_env *env, t_memories *memories)
{
	const char	*path;
	char		*old_pwd;
	char		*new_pwd;

    // Get the current working directory and store it in old_pwd
	old_pwd = getcwd(NULL, 0);
	if (old_pwd == NULL)
	{
		perror("getcwd");
		return (1);
	}
	if (args[0] == NULL || strcmp(args[0], "~") == 0)
	{
		path = get_env_value("HOME", env);
		if (path == NULL)
		{
			fprintf(stderr, "cd: HOME not set\n");
			free(old_pwd);
			return (1);
		}
	}
	else if (strcmp(args[0], "-") == 0)
	{
		path = get_env_value("OLDPWD", env);
		if (path == NULL)
		{
			fprintf(stderr, "cd: OLDPWD not set\n");
			free(old_pwd);
			return (1);
		}
		printf("%s\n", path); // Print the new directory as per convention
	}
	else
	{
		path = args[0];
	}
	if (chdir(path) != 0)
	{
		perror("cd");
		free(old_pwd);
		return (1);
	}
    // Get the new current working directory and store it in new_pwd
	new_pwd = getcwd(NULL, 0);
	if (new_pwd == NULL)
	{
		perror("getcwd");
		free(old_pwd);
		return (1);
	}
    // Update the OLDPWD and PWD environment variables
	add_or_update_env_var(env, "OLDPWD", old_pwd, memories);
	add_or_update_env_var(env, "PWD", new_pwd, memories);
	free(old_pwd);
	free(new_pwd);
	return (0);
}

int	bui_echo(char **args)
{
	int	i;
	int	newline;

	i = 0;
	newline = 1;
	if (args[i] && strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
		{
			printf(" ");
		}
		fflush(stdout);
		i++;
	}
	if (newline)
	{
		printf("\n");
	}
	fflush(stdout);
	return (0);
}

int	bui_exit(char **args)
{
	int	exit_code;
	int	i;

	i = 0;
	exit_code = 0;
	if (args[1] != NULL)
	{
		while (args[1][i])
		{
			if (!isdigit(args[1][i]))
			{
				fprintf(stderr, "exit: numeric argument required\n");
				exit(255);
				i++;
			}
		}
		exit_code = atoi(args[1]);
	}
	exit(exit_code);
}

// int	bui_pwd(void)
// {
// 	char	*cwd;

// 	cwd = getcwd(NULL, 0);
// 	if (!cwd)
// 	{
// 		perror("getcwd");
// 		return (1);
// 	}
// 	printf("%s\n", cwd);
// 	free(cwd);
// 	return (0);
// }

/* I change the function bui_pwd to use the deep_copy_env function to createa copy of
the environment and then search for the PWD variable in the copied environment. 
If the PWD variable is found, the value is printed and the function returns SUCCESS.
If the PWD variable is not found, the function returns FAILURE. 
The copied environment is freed before returning. */

// ************  with copy of enviroment ************

// int	bui_pwd(t_env *env, t_memories *memories)
// {
// 	int		i;
// 	t_env	copied_env;

// 	if (env == NULL || env->pairs == NULL)
// 	{
// 		fprintf(stderr, "pwd: PWD not set\n");
// 		return (FAILURE);
// 	}
// 	copied_env = deep_copy_env(env, memories);
// 	i = 0;
// 	while (i < copied_env.size)
// 	{
// 		if (strncmp(copied_env.pairs[i].key, "PWD", 3) == 0)
// 		{
// 			printf("%s\n", copied_env.pairs[i].value);
// 			return (SUCCESS);
// 			break ;
// 		}
// 		i++;
// 	}
// 	while (i < copied_env.size)
// 	{
// 		free(copied_env.pairs[i].key);
// 		free(copied_env.pairs[i].value);
// 		i++;
// 	}
// 	free(copied_env.pairs);
// 	return (FAILURE);
// }

int	bui_pwd(t_env *env)
{
	int	i;

	if (env == NULL || env->pairs == NULL)
	{
		fprintf(stderr, "pwd: PWD not set\n");
		return (FAILURE);
	}
	i = 0;
	while (i < env->size)
	{
		if (strncmp(env->pairs[i].key, "PWD", 3) == 0)
		{
			printf("%s\n", env->pairs[i].value);
			return (SUCCESS);
			break ;
		}
		i++;
	}
	return (FAILURE);
}
