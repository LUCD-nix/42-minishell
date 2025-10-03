/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_file_pipe_andor.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 17:16:18 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/03 17:16:21 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	get_pipe_exit_status(int status)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			return (130);
		if (WTERMSIG(status) == SIGQUIT)
			return (131);
	}
	return (WEXITSTATUS(status));
}

int	traverse_pipe(t_ast *node)
{
	int		status;
	int		pipe_fd[2];
	pid_t	pids[2];

	pipe_propagate_fd(node);
	if (pipe(pipe_fd) == -1)
		exit_and_free(node, EXIT_FAILURE, "error creating pipe");
	pids[PIPE_LEFT] = fork();
	if (pids[PIPE_LEFT] == -1)
		exit_and_free(node, EXIT_FAILURE, "error forking process");
	if (pids[PIPE_LEFT] == CHILD_PID)
		pipe_left_routine(node, pipe_fd[PIPE_IN], pipe_fd[PIPE_OUT]);
	pids[PIPE_RIGHT] = fork();
	if (pids[PIPE_RIGHT] == -1)
		exit_and_free(node, EXIT_FAILURE, "error forking process");
	if (pids[PIPE_RIGHT] == CHILD_PID)
		pipe_right_routine(node, pipe_fd[PIPE_IN], pipe_fd[PIPE_OUT]);
	close(pipe_fd[PIPE_IN]);
	close(pipe_fd[PIPE_OUT]);
	ignore_signals();
	waitpid(pids[PIPE_LEFT], NULL, 0);
	waitpid(pids[PIPE_RIGHT], &status, 0);
	setup_interactive_signals();
	return (get_pipe_exit_status(status));
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
