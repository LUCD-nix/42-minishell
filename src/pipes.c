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

// TODO : replace all the return (-1) with appropriate error functions
int	pipe_create(t_ast *writer, t_ast *reader)
{
	int	pipe_fd[2];
	int	status;

	status = pipe(pipe_fd);
	if (status == -1)
		return (-1);
	writer->fd_out = pipe_fd[PIPE_IN];
	reader->fd_in = pipe_fd[PIPE_OUT];
	return (0);
}

void	pipe_propagate_fd(t_ast *node)
{
	node->left->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}

int	pipe_from_file(char *file)
{
	int		file_fd;
	int		pipe_fd[2];

	file_fd = open(file, O_RDONLY);
	if (file_fd == -1)
		return (-1);
	if (pipe(pipe_fd) == -1)
		return (-1);
	if (dup2(file_fd, pipe_fd[PIPE_OUT]) == -1)
		return (-1);
	if (close(pipe_fd[PIPE_IN]) == -1)
		return (-1);
	return (pipe_fd[PIPE_OUT]);
}

