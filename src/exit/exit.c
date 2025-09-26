/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 16:01:13 by lucorrei          #+#    #+#             */
/*   Updated: 2025/09/17 16:01:15 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"
#include <unistd.h>

void	close_fds(t_ast *node)
{
	if (node->fd_in != STDIN_FILENO)
	{
		if (close(node->fd_in) == -1)
			perror("error closing in file");
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		if (close(node->fd_out) == -1)
			perror("error closing out file");
	}
	if (!isatty(STDIN_FILENO))
	{
		if (close(STDIN_FILENO) == -1)
			perror("error closing stdin");
	}
	if (!isatty(STDOUT_FILENO))
	{
		if (close(STDOUT_FILENO) == -1)
			perror("error closing stdout");
	}
}

void	exit_and_free(t_ast *node, int exit_code, char *message)
{
	if (exit_code != EXIT_SUCCESS && message != NULL)
	{
		ft_printf("minishell: ");
		perror(message);
	}
	close_fds(node);
	ft_lstclear(node->env, &env_free);
	free_ast(node->top);
	exit(exit_code);
}
