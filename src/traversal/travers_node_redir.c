/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_node_redir_subshell.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/10/02 13:45:31 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	get_redir_flags(t_node_type type)
{
	if (type == NODE_REDIR_IN)
		return (O_RDONLY);
	else if (type == NODE_REDIR_OUT)
		return (O_WRONLY | O_CREAT | O_TRUNC);
	else if (type == NODE_APPEND)
		return (O_WRONLY | O_CREAT | O_APPEND);
	return (0);
}

static t_file_desc	open_redir_file(t_ast *node, int o_flags)
{
	t_file_desc	file_fd;

	if (node->type == NODE_HEREDOC)
	{
		file_fd = traverse_heredoc(node);
		if (file_fd == -1)
			return (-1);
	}
	else
		file_fd = traverse_file(node, o_flags);
	return (file_fd);
}

static int	traverse_next_redir(t_ast *node)
{
	if (node->right == NULL)
		return (traverse_node(node->left));
	node->right->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
	return (traverse_redir(node->right));
}

int	traverse_redir(t_ast *node)
{
	t_file_desc	file_fd;
	int			o_flags;
	int			res;

	o_flags = get_redir_flags(node->type);
	file_fd = open_redir_file(node, o_flags);
	if (file_fd == -1)
	{
		if (g_signal_received == SIGINT)
		{
			g_signal_received = 0;
			return (130);
		}
		return (1);
	}
	redir_propagate_fd(node, file_fd);
	res = traverse_next_redir(node);
	close(file_fd);
	return (res);
}

int	traverse_node(t_ast *node)
{
	int			res;
	t_node_type	type;

	res = -1;
	type = node->type;
	if (type == NODE_CMD)
		res = exec_process(node);
	else if (type == NODE_BUILTIN)
		res = traverse_builtin(node);
	else if (type == NODE_PIPE)
		res = traverse_pipe(node);
	else if (type == NODE_REDIR_IN || type == NODE_HEREDOC
		|| type == NODE_REDIR_OUT || type == NODE_APPEND)
		res = traverse_redir(node);
	else if (type == NODE_AND || type == NODE_OR)
		res = traverse_andor(node, type);
	else if (type == NODE_SUBSHELL)
		res = traverse_subshell(node);
	return (res);
}
