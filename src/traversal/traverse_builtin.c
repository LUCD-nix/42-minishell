/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_file_builtin_pipe_andor_utils.c           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 13:40:29 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/03 00:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <unistd.h>

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
		if (dup2(saved_in, STDIN_FILENO) == -1
			|| close(saved_in) == -1
			|| close(STDIN_FILENO) == -1)
			exit_and_free(node, EXIT_FAILURE, "error restoring stdin");
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		if (dup2(saved_out, STDOUT_FILENO) == -1
			|| close(saved_out) == -1
			|| close(STDOUT_FILENO) == -1)
			exit_and_free(node, EXIT_FAILURE, "error restoring stdout");
	}
	return (res);
}
