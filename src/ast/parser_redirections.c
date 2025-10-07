/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:19 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/07 00:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_ast	*put_cmd_at_end(t_ast *first_redir, t_ast *cmd)
{
	t_ast	*current;

	if (!first_redir)
		return (cmd);
	current = first_redir;
	while (current->right)
		current = current->right;
	current->left = cmd;
	return (first_redir);
}

static t_ast	*add_redir_to_chain(t_ast *chain, t_ast *new_redir)
{
	t_ast	*current;

	if (!chain)
		return (new_redir);
	current = chain;
	while (current->right)
		current = current->right;
	current->right = new_redir;
	return (chain);
}

t_ast	*parse_single_redirection(t_parser *parser, t_list **env)
{
	t_token_type	redir_type;
	t_ast			*redir_node;
	char			*filename;
	char			*cleaned;
	int				has_quotes;

	redir_type = parser->current->type;
	advance(parser);
	if (!check(parser, T_WORD))
		return (error(parser, "Expected filename after redirection"), NULL);
	filename = parser->current->value;
	has_quotes = (parser->current->quote != Q_NONE)
		|| filename_contains_quotes(filename);
	cleaned = clean_filename(filename);
	if (!cleaned)
		return (error(parser, "Memory allocation failed"), NULL);
	advance(parser);
	redir_node = create_redirection_node(redir_type, env);
	if (!redir_node)
		return (free(cleaned), error(parser, "Unknown redirection type"),
			NULL);
	if (redir_node->type == NODE_HEREDOC && has_quotes)
		redir_node->heredoc_quoted = 1;
	redir_node->filename = cleaned;
	return (redir_node);
}

static t_ast	*cmd_with_redir_inner(t_parser *parser, t_list **env,
					t_ast *cmd, t_ast **first_redir)
{
	t_ast	*new_redir;

	if (is_redirection_token(parser))
	{
		new_redir = parse_single_redirection(parser, env);
		if (!new_redir)
			return (free_ast(*first_redir), free_ast(cmd), NULL);
		*first_redir = add_redir_to_chain(*first_redir, new_redir);
	}
	return (*first_redir);
}

t_ast	*parse_command_with_redirections(t_parser *parser, t_list **env)
{
	t_ast	*first_redir;
	t_ast	*cmd;

	first_redir = NULL;
	cmd = NULL;
	while (is_redirection_token(parser) || check(parser, T_WORD))
	{
		if (is_redirection_token(parser))
		{
			if (cmd_with_redir_inner(parser, env, cmd, &first_redir) == NULL)
				return (NULL);
		}
		else if (!cmd && check(parser, T_WORD))
		{
			cmd = parse_command(parser, env);
			if (!cmd)
				return (free_ast(first_redir), NULL);
		}
		else
			break ;
	}
	if (!cmd)
		return (free_ast(first_redir),
			error(parser, "Expected command"), NULL);
	return (put_cmd_at_end(first_redir, cmd));
}
