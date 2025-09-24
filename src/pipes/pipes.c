/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/04 16:25:40 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

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
