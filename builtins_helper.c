/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkaratsi <kkaratsi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 13:46:07 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 13:58:15 by kkaratsi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_unset(t_command *command, t_env *environment)
{
	size_t	i;

	i = 1;
	while (command->args[i] != NULL)
	{
		unset_env_var(environment, command->args[i]);
		i++;
	}
}

void	unset_env_var(t_env *env, const char *key)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (i < env->size)
	{
		if (ft_strcmp(key, "PATH") == 0)
		{
			env->path_unset = 1;
		}
		if (ft_strcmp(env->pairs[i].key, key) == 0)
		{
			free(env->pairs[i].key);
			free(env->pairs[i].value);
			j = i;
			while (j < env->size - 1)
			{
				env->pairs[j] = env->pairs[j + 1];
				j++;
			}
			env->size--;
			return ;
		}
		i++;
	}
}

int	check_n_flag(char **args, int *newline)
{
	int	i;
	int	j;

	*newline = 1;
	i = 0;
	while (args[i])
	{
		if (args[i][0] == '-' && args[i][1] == 'n')
		{
			j = 1;
			while (args[i][j] == 'n')
				j++;
			if (args[i][j] == '\0')
			{
				*newline = 0;
				i++;
			}
			else
				break ;
		}
		else
			break ;
	}
	return (i);
}

int	change_directory(const char *path, char *old_pwd)
{
	if (chdir(path) != 0)
	{
		perror("cd");
		free(old_pwd);
		return (FAILURE);
	}
	return (SUCCESS);
}

char	*get_current_directory(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd == NULL)
	{
		perror("getcwd");
	}
	return (cwd);
}
