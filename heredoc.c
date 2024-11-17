/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 15:39:51 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/17 15:42:45 by pwojnaro         ###   ########.fr       */
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

void	add_token_debug(t_token **head, t_token *new_token)
{
	printf("[DEBUG] Adding token: value='%s', type='%d'\n",
		new_token->value, new_token->type);
	add_token(head, new_token);
}

// Function to add a node to the end of the list
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

void	free_heredoc_list(t_heredoc_node *head)
{
	t_heredoc_node	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp->line);
		free(tmp);
	}
}

int	collect_heredoc_input(const char *delimiter, t_heredoc_node **heredoc_list)
{
	char	*line;
	size_t	len;
	ssize_t	read;

	len = 0;
	line = NULL;
	printf("[DEBUG] Starting heredoc input collection. Delimiter: '%s'\n",
		delimiter);
	printf("heredoc> ");
	while ((read = getline(&line, &len, stdin)) != -1)
	{
		printf("[DEBUG] Read line: %s", line);
		if (line[read - 1] == '\n')
			line[read - 1] = '\0';
		if (strcmp(line, delimiter) == 0)
		{
			printf("[DEBUG] Delimiter matched: '%s'\n", delimiter);
			break ;
		}
		printf("[DEBUG] Adding line to heredoc list: %s\n", line);
		append_heredoc_node(heredoc_list, line);
		printf("heredoc> ");
	}
	if (read == -1)
		printf("[DEBUG] End of input or error while reading heredoc.\n");
	free(line);
	return (0);
}