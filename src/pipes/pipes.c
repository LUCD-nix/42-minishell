/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/10/03 00:00:00 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <stdlib.h>
#include <unistd.h>

void	pipe_left_routine(t_ast *node_pipe, int pipe_in, int pipe_out)
{
	setup_child_signals();
	if (close(pipe_out) == -1)
		exit_and_free(node_pipe, EXIT_FAILURE, "error closing unused pipe");
	if (dup2(pipe_in, STDOUT_FILENO) == -1)
		exit_and_free(node_pipe, EXIT_FAILURE, "error duping to stdout");
	if (close(pipe_in) == -1)
		exit_and_free(node_pipe, EXIT_FAILURE, "error closing out after dup");
	if (node_pipe->fd_out != STDOUT_FILENO)
		if (close(node_pipe->fd_out) == -1)
			exit_and_free(node_pipe, EXIT_FAILURE, "error closing outfile");
	node_pipe->left->fd_out = STDOUT_FILENO;
	exit_and_free(node_pipe->left, traverse_node(node_pipe->left), NULL);
}

void	pipe_right_routine(t_ast *node_pipe, int pipe_in, int pipe_out)
{
	setup_child_signals();
	if (close(pipe_in) == -1)
		exit_and_free(node_pipe, EXIT_FAILURE, "error closing unused pipe");
	if (dup2(pipe_out, STDIN_FILENO) == -1)
		exit_and_free(node_pipe, EXIT_FAILURE, "error duping to stdin");
	if (close(pipe_out) == -1)
		exit_and_free(node_pipe, EXIT_FAILURE, "error closing in after dup");
	if (node_pipe->fd_in != STDIN_FILENO)
		if (close(node_pipe->fd_in) == -1)
			exit_and_free(node_pipe, EXIT_FAILURE, "error closing infile");
	node_pipe->right->fd_in = STDIN_FILENO;
	exit_and_free(node_pipe->right, traverse_node(node_pipe->right), NULL);
}

void	pipe_propagate_fd(t_ast *node)
{
	node->left->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}
