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
		t_ast *current_redir, t_ast *new_redir)
{
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
	return (current_redir);
}

t_ast	*collect_all_redirections(t_parser *parser, t_ast *cmd, t_list **env)
{
	t_ast	*bottom_redir;
	t_ast	*current_redir;
	t_ast	*new_redir;

	bottom_redir = NULL;
	current_redir = NULL;
	while (is_redirection_token(parser))
	{
		new_redir = parse_single_redirection(parser, env);
		if (!new_redir)
		{
			if (current_redir)
				free_ast(current_redir);
			return (NULL);
		}
		current_redir = link_redirection_chain(bottom_redir, current_redir,
				new_redir);
		if (!bottom_redir)
			bottom_redir = current_redir;
	}
	if (bottom_redir)
		bottom_redir->left = cmd;
	else
		return (cmd);
	return (current_redir);
}

static t_ast	*handle_prefix_redirections(t_parser *parser, t_list **env)
{
	if (is_redirection_token(parser))
		return (collect_all_redirections(parser, NULL, env));
	return (NULL);
}

static t_ast	*attach_prefix_to_command(t_ast *prefix_redirs, t_ast *cmd)
{
	t_ast	*temp;

	if (prefix_redirs)
	{
		temp = prefix_redirs;
		while (temp->left)
			temp = temp->left;
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
	result = collect_all_redirections(parser, result, env);
	return (result);
}
