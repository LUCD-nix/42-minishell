/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traversal_funcs.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 10:57:47 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/20 10:57:50 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../minishell.h"

int	traverse_redirect_builtin(t_ast *node, int *saved_in, int *saved_out)
{
	if (node->fd_in != STDIN_FILENO)
	{
		*saved_in = dup(STDIN_FILENO);
		if (*saved_in == -1 || dup2(node->fd_in, STDIN_FILENO) == -1)
			return (-1);
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		*saved_out = dup(STDOUT_FILENO);
		if (*saved_out == -1 || dup2(node->fd_out, STDOUT_FILENO) == -1)
			return (-1);
	}
	return (0);
}

int	traverse_builtin(t_ast *node)
{
	int	saved_in;
	int	saved_out;
	int	res;

	saved_in = 0;
	saved_out = 0;
	if (traverse_redirect_builtin(node, &saved_in, &saved_out) == -1)
		return (-1);
	res = exec_builtin(node);
	if (node->fd_in != STDIN_FILENO)
	{
		if (dup2(saved_in, STDIN_FILENO) == -1)
			return (-1);
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		if (dup2(saved_out, STDOUT_FILENO) == -1)
			return (-1);
	}
	return (res);
}

int	traverse_pipe(t_ast *node)
{
	int	res;

	res = -1;
	pipe_propagate_fd(node);
	if (pipe_create(node->left, node->right) == -1)
		return (-1);
	traverse_node(node->left);
	if (close(node->left->fd_out) == -1)
		return (-1);
	res = traverse_node(node->right);
	if (close(node->right->fd_in) == -1)
		return (-1);
	return (res);
}

int	traverse_andor(t_ast *node, t_node_type type)
{
	int	res;

	res = -1;
	andor_propagate_fd(node);
	res = traverse_node(node->left);
	if (type == NODE_AND)
	{
		if (res == EXIT_SUCCESS)
			res = traverse_node(node->right);
	}
	// TODO : untested (NODE_OR)
	else
	{
		if (res == EXIT_FAILURE)
			res = traverse_node(node->right);
	}
	return (res);
}

int	traverse_file(t_ast *node, int flags)
{
	int	file_fd;

	file_fd = open(node->filename, flags, 0644);
	if (file_fd == -1)
		return (-1);
	if (node->fd_in != STDIN_FILENO
		&& dup2(node->fd_in, file_fd) == -1)
		return (-1);
	if (node->fd_out != STDOUT_FILENO
		&& dup2(node->fd_out, file_fd) == -1)
		return (-1);
	return (file_fd);
}
