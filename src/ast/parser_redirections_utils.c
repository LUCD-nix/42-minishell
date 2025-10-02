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

static t_filename	concat_quoted_tokens(t_parser *parser)
{
	t_filename	result;
	char		*temp;

	result.value = ft_strdup(parser->current->value);
	if (!result.value)
		return ((t_filename){NULL, 0});
	result.has_quotes = (parser->current->quote != Q_NONE);
	advance(parser);
	while (check(parser, T_WORD) && parser->current
		&& parser->current->quote != Q_NONE)
	{
		result.has_quotes = 1;
		temp = ft_strjoin(result.value, parser->current->value);
		free(result.value);
		if (!temp)
			return ((t_filename){NULL, 0});
		result.value = temp;
		advance(parser);
	}
	return (result);
}

t_ast	*parse_single_redirection(t_parser *parser, t_list **env)
{
	t_token_type	redir_type;
	t_ast			*redir_node;
	t_filename		fname;

	redir_type = parser->current->type;
	advance(parser);
	if (!check(parser, T_WORD))
		return (error(parser, "Expected filename after redirection"), NULL);
	fname = concat_quoted_tokens(parser);
	if (!fname.value)
		return (error(parser, "Memory allocation failed"), NULL);
	redir_node = create_redirection_node(redir_type, env);
	if (!redir_node)
	{
		free(fname.value);
		return (error(parser, "Unknown redirection type"), NULL);
	}
	if (redir_node->type == NODE_HEREDOC)
		redir_node->heredoc_quoted = fname.has_quotes;
	redir_node->filename = fname.value;
	return (redir_node);
}

int	is_redirection_token(t_parser *parser)
{
	return (parser->current && (parser->current->type == T_REDIR_IN
			|| parser->current->type == T_REDIR_OUT
			|| parser->current->type == T_APPEND
			|| parser->current->type == T_HEREDOC));
}
