/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traversal.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/04 16:25:40 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../minishell.h"
#include <fcntl.h>
#include <unistd.h>

// TODO : replace all the return (-1) with appropriate error functions
// TODO: (a | b && c | d) < file does not propagate file to "c"
// in BASH, and it does here, needs more testing
void	andor_propagate_fd(t_ast *node)
{
	node->left->fd_in = node->fd_in;
	node->left->fd_out = node->fd_out;
	node->right->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}

int	redirect_builtin(t_ast *node, int *saved_in, int *saved_out)
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
	if (redirect_builtin(node, &saved_in, &saved_out) == -1)
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

void	redir_propagate_fd(t_ast *node)
{
	node->right->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}

int	traverse_node(t_ast *node)
{
	int		res;
	int		file_fd;

	res = -1;
	if (node->type == NODE_CMD)
		res = exec_process(node);
	else if (node->type == NODE_BUILTIN)
		res = traverse_builtin(node);
	else if (node->type == NODE_PIPE)
		res = traverse_pipe(node);
	else if (node->type == NODE_IN)
	{
		redir_propagate_fd(node);
		file_fd = traverse_file(node->right, O_RDONLY);
		if (file_fd == -1)
			return (-1);
		node->left->fd_in = file_fd;
		res = traverse_node(node->left);
		close(file_fd);
	}
	else if (node->type == NODE_OUT)
	{
		redir_propagate_fd(node);
		file_fd = traverse_file(node->right,
			 O_WRONLY | O_TRUNC | O_CREAT);
		if (file_fd == -1)
			return (-1);
		node->left->fd_out = file_fd;
		res = traverse_node(node->left);
		close(file_fd);
	}
	else if (node->type == NODE_APPEND)
	{
		redir_propagate_fd(node);
		file_fd = traverse_file(node->right,
			 O_WRONLY | O_APPEND | O_CREAT);
		if (file_fd == -1)
			return (-1);
		node->left->fd_out = file_fd;
		res = traverse_node(node->left);
		close(file_fd);
	}
	else if (node->type == NODE_AND || node->type  == NODE_OR)
		res = traverse_andor(node, node->type);
	return (res);
}

int main(void)
{
	t_ast	lsl = {
		.type = NODE_CMD,
		.command = &(t_command) {
			.path = "/usr/bin/ls",
			.args = (char *[3]) {"ls", "-la", NULL},
		},
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
	};
	t_ast	file1 = {
		.type = NODE_FILE,
		.filename = "file1",
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
	};
	t_ast	file2 = {
		.type = NODE_FILE,
		.filename = "file2",
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
	};
	t_ast	redir1 = {
		.type = NODE_OUT,
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
		.left = &lsl,
		.right = &file1,
	};
	t_ast	redir2 = {
		.type = NODE_OUT,
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
		.left = &redir1,
		.right = &file2,
	};
	traverse_node(&redir2);
}
