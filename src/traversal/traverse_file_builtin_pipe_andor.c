/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_file_builtin_pipe_andor.c                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 10:57:47 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/28 15:25:52 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"
#include <stdlib.h>

int	traverse_redirect_builtin(t_ast *node, int *saved_in, int *saved_out)
{
	if (node->fd_in != STDIN_FILENO)
	{
		*saved_in = dup(STDIN_FILENO);
		if (*saved_in == -1 || dup2(node->fd_in, STDIN_FILENO) == -1)
			exit_and_free(node, EXIT_FAILURE, "error redirecting stdin");
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		*saved_out = dup(STDOUT_FILENO);
		if (*saved_out == -1 || dup2(node->fd_out, STDOUT_FILENO) == -1)
			exit_and_free(node, EXIT_FAILURE, "error redirecting stdout");
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
	traverse_redirect_builtin(node, &saved_in, &saved_out);
	res = exec_builtin(node);
	if (node->fd_in != STDIN_FILENO)
	{
		if (dup2(saved_in, STDIN_FILENO) == -1)
			exit_and_free(node, EXIT_FAILURE, "error restoring stdin");
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		if (dup2(saved_out, STDOUT_FILENO) == -1)
			exit_and_free(node, EXIT_FAILURE, "error restoring stdout");
	}
	return (res);
}

static void	fork_pipe_left(t_ast *node, int *pipe_fd, pid_t *pids)
{
	pids[PIPE_LEFT] = fork();
	if (pids[PIPE_LEFT] == -1)
		exit_and_free(node, EXIT_FAILURE, "error forking process");
	if (pids[PIPE_LEFT] == CHILD_PID)
	{
		setup_child_signals();
		pipe_left_routine(node, pipe_fd[PIPE_IN], pipe_fd[PIPE_OUT]);
	}
}

static void	fork_pipe_right(t_ast *node, int *pipe_fd, pid_t *pids)
{
	pids[PIPE_RIGHT] = fork();
	if (pids[PIPE_RIGHT] == -1)
		exit_and_free(node, EXIT_FAILURE, "error forking process");
	if (pids[PIPE_RIGHT] == CHILD_PID)
	{
		setup_child_signals();
		pipe_right_routine(node, pipe_fd[PIPE_IN], pipe_fd[PIPE_OUT]);
	}
}

int	traverse_pipe(t_ast *node)
{
	int		res;
	int		pipe_fd[2];
	pid_t	pids[2];

	pipe_propagate_fd(node);
	if (pipe(pipe_fd) == -1)
		exit_and_free(node, EXIT_FAILURE, "error creating pipe");
	fork_pipe_left(node, pipe_fd, pids);
	fork_pipe_right(node, pipe_fd, pids);
	ignore_signals();
	res = pipe_wait_for_children(pids, pipe_fd);
	setup_interactive_signals();
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
	else
	{
		if (res != EXIT_FAILURE)
			res = traverse_node(node->right);
	}
	return (res);
}

t_file_desc	traverse_file(t_ast *node, int flags)
{
	int	file_fd;

	file_fd = open(node->filename, flags, 0644);
	if (file_fd == -1)
		exit_and_free(node, EXIT_FAILURE, "error opening file");
	return (file_fd);
}
