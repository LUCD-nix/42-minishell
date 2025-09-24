/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_operations.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:23 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/24 14:43:14 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*parse_subshell(t_parser *parser, t_list **env)
{
	t_ast	*node;

	advance(parser);
	node = init_ast_node(NODE_SUBSHELL, env);
	if (!node)
		return (error(parser, "Memory allocation failed"), NULL);
	node->left = parse_expression(parser, PREC_NONE, env);
	if (!node->left)
	{
		error(parser, "Expected expression in subshell");
		return (free_ast(node), NULL);
	}
	if (!check(parser, T_RPAREN))
	{
		error(parser, "Expected ')' after subshell");
		return (free_ast(node), NULL);
	}
	advance(parser);
	return (node);
}

t_ast	*parse_binary(t_parser *parser, t_ast *left, t_token_type op,
		t_list **env)
{
	t_ast		*node;
	t_node_type	node_type;

	if (op == T_PIPE)
		node_type = NODE_PIPE;
	else if (op == T_AND)
		node_type = NODE_AND;
	else if (op == T_OR)
		node_type = NODE_OR;
	else
		return (error(parser, "Unknown binary operator"), left);
	node = init_ast_node(node_type, env);
	if (!node)
		return (error(parser, "Memory allocation failed"), left);
	node->left = left;
	node->right = parse_expression(parser, get_precedence(op) + 1, env);
	if (!node->right)
		return (free_ast(node), NULL);
	return (node);
}

t_ast	*parse_primary(t_parser *parser, t_list **env)
{
	if (check(parser, T_LPAREN))
		return (parse_subshell(parser, env));
	if (check(parser, T_WORD) || (parser->current
			&& (parser->current->type == T_REDIR_IN
				|| parser->current->type == T_REDIR_OUT
				|| parser->current->type == T_APPEND
				|| parser->current->type == T_HEREDOC)))
		return (parse_command_with_redirections(parser, env));
	error(parser, "Expected command or '('");
	return (NULL);
}


