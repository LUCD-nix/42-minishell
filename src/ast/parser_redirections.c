/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:19 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:27:05 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_ast	*link_redirection_chain(t_ast *bottom_redir,
		t_ast *new_redir, t_ast **top_redir)
{
	if (!*top_redir)
	{
		bottom_redir = new_redir;
		*top_redir = new_redir;
	}
	else
	{
		bottom_redir->right = new_redir;
		bottom_redir = new_redir;
	}
	return (bottom_redir);
}

t_ast	*collect_all_redirections(t_parser *parser, t_ast *cmd, t_list **env,
		t_ast *top_redir)
{
	t_ast	*bottom_redir;
	t_ast	*new_redir;

	bottom_redir = top_redir;
	while (top_redir != NULL && bottom_redir->right != NULL)
		bottom_redir = bottom_redir->right;
	if (cmd && bottom_redir != NULL)
		bottom_redir->left = NULL;
	while (is_redirection_token(parser))
	{
		new_redir = parse_single_redirection(parser, env);
		if (!new_redir)
		{
			free_ast(top_redir);
			return (NULL);
		}
		bottom_redir = link_redirection_chain(bottom_redir,
				new_redir, &top_redir);
	}
	if (bottom_redir)
		bottom_redir->left = cmd;
	else
		return (cmd);
	return (top_redir);
}

static t_ast	*handle_prefix_redirections(t_parser *parser, t_list **env)
{
	if (is_redirection_token(parser))
		return (collect_all_redirections(parser, NULL, env, NULL));
	return (NULL);
}

static t_ast	*attach_prefix_to_command(t_ast *prefix_redirs, t_ast *cmd)
{
	t_ast	*temp;

	if (prefix_redirs)
	{
		temp = prefix_redirs;
		while (temp->right)
			temp = temp->right;
		temp->left = cmd;
		return (prefix_redirs);
	}
	return (cmd);
}

t_ast	*parse_command_with_redirections(t_parser *parser, t_list **env)
{
	t_ast	*cmd;
	t_ast	*result;
	t_ast	*prefix_redirs;

	prefix_redirs = handle_prefix_redirections(parser, env);
	if (!prefix_redirs && !check(parser, T_WORD))
		return (error(parser, "Expected command"), NULL);
	if (!check(parser, T_WORD))
	{
		if (prefix_redirs)
			free_ast(prefix_redirs);
		return (error(parser, "Expected command"), NULL);
	}
	cmd = parse_command(parser, env);
	if (!cmd)
	{
		if (prefix_redirs)
			free_ast(prefix_redirs);
		return (NULL);
	}
	result = attach_prefix_to_command(prefix_redirs, cmd);
	result = collect_all_redirections(parser, cmd, env, prefix_redirs);
	return (result);
}
