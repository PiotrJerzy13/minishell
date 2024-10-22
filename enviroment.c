/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 13:44:08 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/14 15:41:48 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	allocate_environment(t_env *enviroment, int size, t_memories *memories)
{
	enviroment->key = (char **)calloc(size + 1, sizeof(char *));
	enviroment->value = (char **)calloc(size + 1, sizeof(char *));
	if (!enviroment->key || !enviroment->value)
	{
		return (1);
	}
	add_memory(memories, enviroment->key);
	add_memory(memories, enviroment->value);
	return (0);
}

char	*allocate_user_input(t_memories *memories)
{
	int		capacity;
	char	*input;
	int		length;
	int		ch;

	capacity = INITIAL_CAPACITY;
	input = malloc(capacity * sizeof(char));
	if (!input)
	{
		printf("Error: Failed to allocate memory for user input.\n");
		exit(EXIT_FAILURE);
	}
	add_memory(memories, input);
	length = 0;
	ch = getchar();
	while (ch != '\n' && ch != EOF)
	{
		if (length >= capacity - 1)
		{
			capacity *= 2;
			input = realloc(input, capacity * sizeof(char));
			if (!input)
			{
				printf("Error: Failed to expand memory for user input.\n");
				exit(EXIT_FAILURE);
			}
			add_memory(memories, input);
		}
		input[length++] = (char) ch;
		ch = getchar();
	}
	input[length] = '\0';
	return (input);
}

void	copy_environment_to_list(char **env, t_env_node **env_list,
t_memories *memories)
{
	int		i;
	char	*key;
	char	*value_copy;
	char	*key_copy;
	char	*value;

	i = 0;
	while (env[i])
	{
		key = strtok(env[i], "=");
		value = strtok(NULL, "=");
		if (key && value)
		{
			key_copy = strdup(key);
			value_copy = strdup(value);
			add_memory(memories, key_copy);
			add_memory(memories, value_copy);
			add_or_update_env_var(env_list, key_copy, value_copy);
		}
		i++;
	}
}

void	add_or_update_env_var(t_env_node **head, char *key, char *value)
{
	t_env_node	*current;
	t_env_node	*new_node;

	current = *head;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
		{
			free(current->value);
			current->value = strdup(value);
			return ;
		}
		current = current->next;
	}
	new_node = (t_env_node *)malloc(sizeof(t_env_node));
	new_node->key = strdup(key);
	new_node->value = strdup(value);
	new_node->next = *head;
	*head = new_node;
}

void	unset_env_var(t_env_node **head, char *key)
{
	t_env_node	*current;
	t_env_node	*previous;

	current = *head;
	previous = NULL;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
		{
			if (previous)
				previous->next = current->next;
			else
				*head = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return ;
		}
		previous = current;
		current = current->next;
	}
}
