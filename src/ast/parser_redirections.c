/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:19 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/24 14:38:20 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast *parse_single_redirection(t_parser *parser, t_list **env)
{
    t_token_type redir_type;
    t_ast *redir_node;

    redir_type = parser->current->type;
    advance(parser);
    if (!check(parser, T_WORD))
        return (error(parser, "Expected filename after redirection"), NULL);

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
            redir_node->heredoc_quoted = (parser->current->quote != Q_NONE);
    }
    else
        return (error(parser, "Unknown redirection type"), NULL);

    if (!redir_node)
        return (error(parser, "Memory allocation failed"), NULL);

    redir_node->filename = ft_strdup(parser->current->value);
    if (!redir_node->filename)
        return (free_ast(redir_node), error(parser, "Memory allocation failed"), NULL);

    advance(parser);
    return (redir_node);
}

t_ast	*reorganize_redirections(t_ast *node)
{
	if (!node)
		return (NULL);
	if (node->type != NODE_REDIR_IN && node->type != NODE_REDIR_OUT
		&& node->type != NODE_APPEND && node->type != NODE_HEREDOC)
		return (node);

	// Pour les redirections output multiples, garder l'ordre naturel
	// (la dernière parsée = la dernière dans la commande = celle qui gagne)
	return (node);
}

t_ast	*collect_all_redirections(t_parser *parser, t_ast *cmd, t_list **env)
{
	t_ast	*bottom_redir;
	t_ast	*current_redir;
	t_ast	*new_redir;

	bottom_redir = NULL;
	current_redir = NULL;
	while (parser->current && (parser->current->type == T_REDIR_IN ||
			parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND ||
			parser->current->type == T_HEREDOC))
	{
		new_redir = parse_single_redirection(parser, env);
		if (!new_redir)
			return (current_redir ? free_ast(current_redir) : NULL, NULL);
		if (!bottom_redir)
		{
			bottom_redir = new_redir;
			current_redir = bottom_redir;
		}
		else
		{
			new_redir->left = current_redir;
			current_redir = new_redir;
		}
	}
	if (bottom_redir)
		bottom_redir->left = cmd;
	else
		return (cmd);
	return (current_redir);
}

t_ast	*parse_command_with_redirections(t_parser *parser, t_list **env)
{
	t_ast	*cmd;
	t_ast	*result;
	t_ast	*prefix_redirs;

	prefix_redirs = NULL;
	if (parser->current && (parser->current->type == T_REDIR_IN ||
			parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND ||
			parser->current->type == T_HEREDOC))
	{
		prefix_redirs = collect_all_redirections(parser, NULL, env);
		if (!prefix_redirs)
			return (NULL);
	}
	if (!check(parser, T_WORD))
		return (prefix_redirs ? free_ast(prefix_redirs) : NULL,
			error(parser, "Expected command"), NULL);
	cmd = parse_command(parser, env);
	if (!cmd)
		return (prefix_redirs ? free_ast(prefix_redirs) : NULL, NULL);
	if (prefix_redirs)
	{
		t_ast *temp = prefix_redirs;
		while (temp->left)
			temp = temp->left;
		temp->left = cmd;
		result = prefix_redirs;
	}
	else
		result = cmd;
	result = collect_all_redirections(parser, result, env);
	return (result);
}
