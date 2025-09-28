/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:58:17 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 14:06:51 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_token_type	get_token_type(char *token)
{
	if (!token)
		return (T_EOF);
	if (ft_strncmp(token, "||", 2) == 0)
		return (T_OR);
	if (ft_strncmp(token, "&&", 2) == 0)
		return (T_AND);
	if (ft_strncmp(token, ">>", 2) == 0)
		return (T_APPEND);
	if (ft_strncmp(token, "<<", 2) == 0)
		return (T_HEREDOC);
	if (ft_strncmp(token, "|", 1) == 0)
		return (T_PIPE);
	if (ft_strncmp(token, "(", 1) == 0)
		return (T_LPAREN);
	if (ft_strncmp(token, ")", 1) == 0)
		return (T_RPAREN);
	if (ft_strncmp(token, ">", 1) == 0)
		return (T_REDIR_OUT);
	if (ft_strncmp(token, "<", 1) == 0)
		return (T_REDIR_IN);
	return (T_WORD);
}

static t_token	*create_token_from_lexeme(t_lexeme *lex, int i)
{
	return (init_token(lex[i].value, get_token_type(lex[i].value),
			lex[i].quote));
}

static void	link_token_to_list(t_token **head, t_token **current,
		t_token *new_node)
{
	if (!*head)
	{
		*head = new_node;
		*current = new_node;
	}
	else
	{
		(*current)->next = new_node;
		*current = (*current)->next;
	}
}

static int	process_lexeme_at_index(t_lexeme *lex, int i, t_token **head,
		t_token **current)
{
	t_token	*new_node;

	new_node = create_token_from_lexeme(lex, i);
	if (!new_node)
	{
		free_token(*head);
		return (0);
	}
	link_token_to_list(head, current, new_node);
	return (1);
}

t_token	*lexer_to_token(t_lexeme *lex)
{
	t_token	*head;
	t_token	*current;
	int		i;

	if (!lex)
		return (NULL);
	head = NULL;
	current = NULL;
	i = 0;
	while (lex[i].value)
	{
		if (!process_lexeme_at_index(lex, i, &head, &current))
			return (NULL);
		i++;
	}
	return (head);
}
