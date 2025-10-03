/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_node_redir_subshell_utils.c               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 13:44:22 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/02 13:45:52 by alvanaut         ###   ########.fr       */
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

int	handle_subshell_signal(int res)
{
	if (WIFSIGNALED(res))
	{
		if (WTERMSIG(res) == SIGINT)
			return (130);
		if (WTERMSIG(res) == SIGQUIT)
			return (131);
	}
	return (WEXITSTATUS(res));
}

void	exec_subshell_child(t_ast *node)
{
	setup_child_signals();
	node->left->fd_in = node->fd_in;
	node->left->fd_out = node->fd_out;
	exit_and_free(node->left, traverse_node(node->left), NULL);
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
		exec_subshell_child(node);
	ignore_signals();
	waitpid(status, &res, 0);
	setup_interactive_signals();
	return (handle_subshell_signal(res));
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
