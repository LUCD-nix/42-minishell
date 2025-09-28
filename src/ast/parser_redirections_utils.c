/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:26:42 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:27:25 by alvanaut         ###   ########.fr       */
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

int	set_heredoc_quote_status(t_ast *redir_node, t_parser *parser)
{
	if (redir_node->type == NODE_HEREDOC)
		redir_node->heredoc_quoted = (parser->current->quote != Q_NONE);
	return (1);
}

t_ast	*parse_single_redirection(t_parser *parser, t_list **env)
{
	t_token_type	redir_type;
	t_ast			*redir_node;

	redir_type = parser->current->type;
	advance(parser);
	if (!check(parser, T_WORD))
		return (error(parser, "Expected filename after redirection"), NULL);
	redir_node = create_redirection_node(redir_type, env);
	if (!redir_node)
		return (error(parser, "Unknown redirection type"), NULL);
	set_heredoc_quote_status(redir_node, parser);
	redir_node->filename = ft_strdup(parser->current->value);
	if (!redir_node->filename)
	{
		free_ast(redir_node);
		return (error(parser, "Memory allocation failed"), NULL);
	}
	advance(parser);
	return (redir_node);
}

int	is_redirection_token(t_parser *parser)
{
	return (parser->current && (parser->current->type == T_REDIR_IN
			|| parser->current->type == T_REDIR_OUT
			|| parser->current->type == T_APPEND
			|| parser->current->type == T_HEREDOC));
}
