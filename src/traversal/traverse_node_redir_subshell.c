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
#include "../../minishell.h"

void	andor_propagate_fd(t_ast *node)
{
	node->left->fd_in = node->fd_in;
	node->left->fd_out = node->fd_out;
	node->right->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}

int	traverse_subshell(t_ast *node)
{
	int	status;
	int	res;

	status = fork();
	res = -1;
	if (status == -1)
		exit_and_free(node, EXIT_FAILURE, "error forking subshell");
	if (status == CHILD_PID)
	{
		node->left->fd_in = node->fd_in;
		node->left->fd_out = node->fd_out;
		return (exit_and_free(node->left, traverse_node(node->left), NULL), 0);
	}
	else
		waitpid(status, &res, 0);
	return (res);
}

void	redir_propagate_fd(t_ast *node, int file_fd)
{
	if (node->type == NODE_REDIR_IN || node->type == NODE_HEREDOC)
	{
		if (node->left != NULL)
		{
			node->left->fd_in = file_fd;
			node->left->fd_out = node->fd_out;
		}
		if (node->fd_in != STDIN_FILENO && close(node->fd_in) == -1)
			exit_and_free(node, EXIT_FAILURE, "error redirecting input");
		node->fd_in = file_fd;
	}
	else
	{
		if (node->left != NULL)
		{
			node->left->fd_out = file_fd;
			node->left->fd_in = node->fd_in;
		}
		if (node->fd_out != STDOUT_FILENO && close(node->fd_out) == -1)
			exit_and_free(node, EXIT_FAILURE, "error redirecting output");
		node->fd_out = file_fd;
	}
}

int	traverse_redir(t_ast *node)
{
	t_file_desc	file_fd;
	int			o_flags;
	int			res;

	if (node->type == NODE_REDIR_IN)
		o_flags = O_RDONLY;
	else if (node->type == NODE_REDIR_OUT)
		o_flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (node->type == NODE_APPEND)
		o_flags = O_WRONLY | O_CREAT | O_APPEND;
	if (node->type == NODE_HEREDOC)
		file_fd = traverse_heredoc(node);
	else
		file_fd = traverse_file(node, o_flags);
	redir_propagate_fd(node, file_fd);
	if (node->right == NULL)
		res = traverse_node(node->left);
	else
	{
		node->right->fd_in = node->fd_in;
		node->right->fd_out = node->fd_out;
		res = traverse_redir(node->right);
	}
	return (close(file_fd), res);
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
//
// int main(void)
// {
// 	extern char **environ;
//
// 	t_list *env = env_lst_from_str_arr(environ);
//
// 	t_ast	lsla = {
// 		.type = NODE_CMD,
// 		.command = &(t_command) {
// 			.path = "/usr/bin/ls",
// 			.args = (char *[3]) {"ls", "-la", NULL},
// 		},
// 		.env = &env,
// 		.fd_in = STDIN_FILENO,
// 		.fd_out = STDOUT_FILENO,
// 	};
// 	t_ast	wcc = {
// 		.type = NODE_CMD,
// 		.command = &(t_command) {
// 			.path = "/usr/bin/wc",
// 			.args = (char *[3]) {"wc", "-c", NULL},
// 		},
// 		.env = &env,
// 		.fd_in = STDIN_FILENO,
// 		.fd_out = STDOUT_FILENO,
// 	};
// 	t_ast	factor = {
// 		.type = NODE_CMD,
// 		.command = &(t_command) {
// 			.path = "/usr/bin/factor",
// 			.args = (char *[2]) {"factor", NULL},
// 		},
// 		.env = &env,
// 		.fd_in = STDIN_FILENO,
// 		.fd_out = STDOUT_FILENO,
// 	};
// 	t_ast pipe1 = {
// 		.type = NODE_PIPE,
// 		.fd_in = STDIN_FILENO,
// 		.fd_out = STDOUT_FILENO,
// 		.left = &lsla,
// 		.right = &wcc,
// 	};
// 	t_ast pipe2  = {
// 		.type = NODE_PIPE,
// 		.fd_in = STDIN_FILENO,
// 		.fd_out = STDOUT_FILENO,
// 		.left = &pipe1,
// 		.right = &factor,
// 	};
// 	traverse_node(&pipe2);
// 	ft_lstclear(&env, env_free);
// }
