/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 15:39:51 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/10 21:28:06 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_heredoc_node	*create_heredoc_node(const char *line, t_memories *memories)
{
	t_heredoc_node	*node;

	node = malloc(sizeof(t_heredoc_node));
	if (!node)
	{
		perror("Error allocating memory for heredoc node");
		return (NULL);
	}
	add_memory(memories, node);
	node->line = ft_strndup(line, ft_strlen(line), memories);
	if (!node->line)
	{
		perror("Error allocating memory for line");
		return (NULL);
	}
	node->next = NULL;
	return (node);
}

void	append_heredoc_node(t_heredoc_node **head, const char *line,
			t_memories *memories)
{
	t_heredoc_node	*new_node;
	t_heredoc_node	*current;

	new_node = create_heredoc_node(line, memories);
	if (!new_node)
		return ;
	if (!*head)
	{
		*head = new_node;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_node;
}

int	collect_heredoc_input(const char *delimiter, t_heredoc_node **heredoc_list,
	t_memories *memories)
{
	char	*line;
	size_t	len;
	ssize_t	read;

	line = NULL;
	len = 0;
	printf("heredoc> ");
	read = getline(&line, &len, stdin);
	while (read != -1)
	{
		if (line[read - 1] == '\n')
			line[read - 1] = '\0';
		if (ft_strcmp(line, delimiter) == 0)
			break ;
		append_heredoc_node(heredoc_list, line, memories);
		printf("heredoc> ");
		read = getline(&line, &len, stdin);
	}
	free(line);
	if (read == -1)
		return (-1);
	return (0);
}

int	handle_heredoc(t_token **current_token, t_command *current_command,
	t_memories *memories)
{
	if (!current_token || !*current_token || !current_command)
	{
		printf("Error: Invalid arguments to handle_heredoc.\n");
		return (-1);
	}
	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		current_command->heredoc_list = NULL;
		if (collect_heredoc_input((*current_token)->value,
				&current_command->heredoc_list, memories) == -1)
		{
			printf("Error collecting heredoc input\n");
			return (-1);
		}
	}
	else
	{
		printf("Syntax error: Expected delimiter after <<\n");
		return (-1);
	}
	return (0);
}
