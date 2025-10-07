/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:26:42 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/07 00:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*create_redirection_node(t_token_type redir_type, t_list **env)
{
	t_ast	*redir_node;

	if (redir_type == T_REDIR_IN)
		redir_node = init_ast_node(NODE_REDIR_IN, env);
	else if (redir_type == T_REDIR_OUT)
		redir_node = init_ast_node(NODE_REDIR_OUT, env);
	else if (redir_type == T_APPEND)
		redir_node = init_ast_node(NODE_APPEND, env);
	else if (redir_type == T_HEREDOC)
	{
		redir_node = init_ast_node(NODE_HEREDOC, env);
		if (redir_node)
			redir_node->heredoc_quoted = 0;
	}
	else
		return (NULL);
	return (redir_node);
}

int	filename_contains_quotes(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

int	is_redirection_token(t_parser *parser)
{
	return (parser->current && (parser->current->type == T_REDIR_IN
			|| parser->current->type == T_REDIR_OUT
			|| parser->current->type == T_APPEND
			|| parser->current->type == T_HEREDOC));
}

char	*clean_filename(char *raw_filename)
{
	char	*cleaned;
	int		i;
	int		j;
	char	quote;

	cleaned = ft_calloc(ft_strlen(raw_filename) + 1, 1);
	if (!cleaned)
		return (NULL);
	i = 0;
	j = 0;
	while (raw_filename[i])
	{
		if (raw_filename[i] == '\'' || raw_filename[i] == '"')
		{
			quote = raw_filename[i++];
			while (raw_filename[i] && raw_filename[i] != quote)
				cleaned[j++] = raw_filename[i++];
			if (raw_filename[i] == quote)
				i++;
		}
		else
			cleaned[j++] = raw_filename[i++];
	}
	cleaned[j] = '\0';
	return (cleaned);
}
