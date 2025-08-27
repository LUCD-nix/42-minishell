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

void	redir_propagate_fd(t_ast *node)
{
	if (node->type == NODE_IN)
	{
		node->right->fd_in = node->fd_in;
		node->left->fd_out = node->fd_out;
	}
	else
	{
		node->left->fd_in = node->fd_in;
		node->right->fd_out = node->fd_out;
	}
}

int	traverse_redir(t_ast *node)
{
	int	file_fd;
	int	o_flags;
	int	res;

	redir_propagate_fd(node);
	if (node->type == NODE_IN)
		o_flags = O_RDONLY;
	else if (node->type == NODE_OUT)
		o_flags = O_WRONLY | O_CREAT | O_TRUNC;
	else
		o_flags = O_WRONLY | O_CREAT | O_APPEND;
	file_fd = traverse_file(node->right, o_flags);
	if (file_fd == -1)
		return (-1);
	if (node->type == NODE_IN)
		node->left->fd_in = file_fd;
	else
		node->left->fd_out = file_fd;
	res = traverse_node(node->left);
	close(file_fd);
	return (res);
}

int	traverse_node(t_ast *node)
{
	int			res;
	t_node_type	type;

	// TODO : Heredoc
	res = -1;
	type = node->type;
	if (type == NODE_CMD)
		res = exec_process(node);
	else if (type == NODE_BUILTIN)
		res = traverse_builtin(node);
	else if (type == NODE_PIPE)
		res = traverse_pipe(node);
	else if (type == NODE_IN || type == NODE_OUT || type == NODE_APPEND)
		res = traverse_redir(node);
	else if (type == NODE_AND || type == NODE_OR)
		res = traverse_andor(node, type);
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
