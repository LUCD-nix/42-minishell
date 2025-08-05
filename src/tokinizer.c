/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokinizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:02:23 by alvanaut          #+#    #+#             */
/*   Updated: 2025/08/04 17:21:36 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*read_line(void)
{
	char	*line;

	line = readline("Minishell$ ");
	if (!line)
		return (NULL);
	if (*line)
		add_history(line);
	return (line);
}

/* static int	word_count(const char *line)
{
	int	i;
	int	count;
	int	in_word;

	i = 0;
	count = 0;
	in_word = 0;
	while (line[i])
	{
		if (!in_word && !ft_isspace(line[i]))
		{
			in_word = 1;
			count++;
		}
		if (in_word && ft_isspace(line[i]))
			in_word = 0;
		i++;
	}
	return (count);
} */

t_token_type	get_token_type(const char *str)
{
	if (strcmp(str, "|") == 0)
		return (T_PIPE);
	else if (strcmp(str, "&&") == 0)
		return (T_AND);
	else if (strcmp(str, "||") == 0)
		return (T_OR);
	else if (strcmp(str, "<") == 0)
		return (T_REDIR_IN);
	else if (strcmp(str, ">") == 0)
		return (T_REDIR_OUT);
	else if (strcmp(str, ">>") == 0)
		return (T_APPEND);
	else if (strcmp(str, "<<") == 0)
		return (T_HEREDOC);
	else if (strcmp(str, "(") == 0)
		return (T_LPAREN);
	else if (strcmp(str, ")") == 0)
		return (T_RPAREN);
	else
		return (T_WORD);
}

t_token	*word_to_token(const char **tab)
{
	t_token	*head;
	t_token	*current;
	t_token	*new;
	int		i;

	head = NULL;
	current = NULL;
	i = 0;
	while (tab[i])
	{
		new = init_token((char *)tab[i], get_token_type(tab[i]));
		if (!new)
		{
			free_token(head);
			return (NULL);
		}
		if (!head)
			head = new;
		else
			current->next = new;
		current = new;
		i++;
	}
	return (head);
}
