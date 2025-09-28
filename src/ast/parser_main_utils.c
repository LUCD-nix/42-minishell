/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_main_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:11:59 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:13:36 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_redirection_operator(t_token_type op_type)
{
	return (op_type == T_REDIR_IN || op_type == T_REDIR_OUT
		|| op_type == T_APPEND || op_type == T_HEREDOC);
}

int	is_binary_operator(t_token_type op_type)
{
	return (op_type == T_PIPE || op_type == T_AND || op_type == T_OR);
}

t_ast	*handle_redirection_operator(t_parser *parser, t_ast *left,
		t_list **env)
{
	t_ast	*redir;

	redir = parse_single_redirection(parser, env);
	if (redir)
	{
		redir->left = left;
		left = redir;
	}
	return (left);
}

t_ast	*handle_binary_operator(t_parser *parser, t_ast *left,
		t_token_type op_type, t_list **env)
{
	advance(parser);
	left = parse_binary(parser, left, op_type, env);
	return (left);
}

t_ast	*process_operator(t_parser *parser, t_ast *left, t_token_type op_type,
		t_list **env)
{
	if (is_redirection_operator(op_type))
		return (handle_redirection_operator(parser, left, env));
	else if (is_binary_operator(op_type))
		return (handle_binary_operator(parser, left, op_type, env));
	return (left);
}
