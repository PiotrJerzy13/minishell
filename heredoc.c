/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 15:39:51 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/23 14:42:07 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_heredoc_node	*create_heredoc_node(const char *line)
{
	t_heredoc_node	*node;

	node = malloc(sizeof(t_heredoc_node));
	if (!node)
	{
		perror("Error allocating memory for heredoc node");
		return (NULL);
	}
	node->line = strdup(line);
	if (!node->line)
	{
		perror("Error allocating memory for line");
		free(node);
		return (NULL);
	}
	node->next = NULL;
	return (node);
}

void	append_heredoc_node(t_heredoc_node **head, const char *line)
{
	t_heredoc_node	*new_node;
	t_heredoc_node	*current;

	new_node = create_heredoc_node(line);
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

int	collect_heredoc_input(const char *delimiter, t_heredoc_node **heredoc_list)
{
	char	*line;
	size_t	len;
	ssize_t	read;

	len = 0;
	line = NULL;
	printf("heredoc> ");
	while (1)
	{
		read = getline(&line, &len, stdin);
		if (read == -1)
			break ;
		if (line[read - 1] == '\n')
			line[read - 1] = '\0';
		if (strcmp(line, delimiter) == 0)
			break ;
		append_heredoc_node(heredoc_list, line);
		printf("heredoc> ");
	}
	if (read == -1)
		free(line);
	return (0);
}

void	handle_heredoc(t_token **current_token, t_command *current_command)
{
	*current_token = (*current_token)->next;
	if (*current_token && (*current_token)->type == TOKEN_FILENAME)
	{
		current_command->heredoc_list = NULL;
		if (collect_heredoc_input((*current_token)->value,
				&current_command->heredoc_list) == -1)
		{
			fprintf(stderr, "Error collecting heredoc input\n");
		}
	}
}
