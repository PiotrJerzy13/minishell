/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 13:46:07 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/11 00:23:26 by pwojnaro         ###   ########.fr       */
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

// void	unset_env_var(t_env *env, const char *key)
// {
// 	size_t	i;
// 	size_t	j;

// 	i = 0;
// 	while (i < env->size)
// 	{
// 		if (ft_strcmp(key, "PATH") == 0)
// 			env->path_unset = 1;
// 		if (ft_strcmp(env->pairs[i].key, key) == 0)
// 		{
// 			if (env->memories)
// 			{
// 				if (is_in_memories(env->memories, env->pairs[i].key))
// 					remove_memory(env->memories, env->pairs[i].key);
// 				if (is_in_memories(env->memories, env->pairs[i].value))
// 					remove_memory(env->memories, env->pairs[i].value);
// 			}
// 			free(env->pairs[i].key);
// 			free(env->pairs[i].value);
// 			j = i;
// 			while (j < env->size - 1)
// 			{
// 				env->pairs[j] = env->pairs[j + 1];
// 				j++;
// 			}
// 			env->size--;
// 			return ;
// 		}
// 		i++;
// 	}
// }
void	free_and_shift_env_pair(t_env *env, size_t index)
{
	size_t	j;

	if (env->memories)
	{
		if (is_in_memories(env->memories, env->pairs[index].key))
			remove_memory(env->memories, env->pairs[index].key);
		if (is_in_memories(env->memories, env->pairs[index].value))
			remove_memory(env->memories, env->pairs[index].value);
	}
	free(env->pairs[index].key);
	free(env->pairs[index].value);
	j = index;
	while (j < env->size - 1)
	{
		env->pairs[j] = env->pairs[j + 1];
		j++;
	}
}

void	unset_env_var(t_env *env, const char *key)
{
	size_t	i;

	i = 0;
	while (i < env->size)
	{
		if (ft_strcmp(key, "PATH") == 0)
			env->path_unset = 1;
		if (ft_strcmp(env->pairs[i].key, key) == 0)
		{
			free_and_shift_env_pair(env, i);
			env->size--;
			return ;
		}
		i++;
	}
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
