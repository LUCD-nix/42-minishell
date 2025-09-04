/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traversal_funcs.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 10:57:47 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/28 15:25:52 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

int	traverse_redirect_builtin(t_ast *node, int *saved_in, int *saved_out)
{
	if (node->fd_in != STDIN_FILENO)
	{
		*saved_in = dup(STDIN_FILENO);
		if (*saved_in == -1 || dup2(node->fd_in, STDIN_FILENO) == -1)
			return (-1);
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		*saved_out = dup(STDOUT_FILENO);
		if (*saved_out == -1 || dup2(node->fd_out, STDOUT_FILENO) == -1)
			return (-1);
	}
	return (0);
}

int	traverse_builtin(t_ast *node)
{
	int	saved_in;
	int	saved_out;
	int	res;

	saved_in = -1;
	saved_out = -1;
	if (traverse_redirect_builtin(node, &saved_in, &saved_out) == -1)
		return (-1);
	res = exec_builtin(node);
	
	// Restaurer les fd originaux
	if (saved_in != -1)
	{
		if (dup2(saved_in, STDIN_FILENO) == -1)
			return (-1);
		close(saved_in);
	}
	if (saved_out != -1)
	{
		if (dup2(saved_out, STDOUT_FILENO) == -1)
			return (-1);
		close(saved_out);
	}
	return (res);
}

int	traverse_pipe(t_ast *node)
{
	int		status_left, status_right;
	pid_t	pid_left;
	pid_t	pid_right;

	pipe_propagate_fd(node);
	if (pipe_create(node->left, node->right) == -1)
		return (-1);
	
	pid_left = fork();
	if (pid_left == -1)
		return (-1);
	if (pid_left == CHILD_PID)
	{
		setup_child_signals();
		// FIX: Fermer le fd de lecture dans l'enfant gauche
		close(node->right->fd_in);
		exit(traverse_node_new(node->left));
	}
	
	if (close(node->left->fd_out) == -1)
		return (-1);
	
	pid_right = fork();
	if (pid_right == -1)
		return (-1);
	if (pid_right == CHILD_PID)
	{
		setup_child_signals();
		exit(traverse_node_new(node->right));
	}
	
	if (close(node->right->fd_in) == -1)
		return (-1);
	
	// FIX: Attendre les deux processus et gérer les erreurs
	waitpid(pid_left, &status_left, 0);
	waitpid(pid_right, &status_right, 0);
	
	// Si la commande de gauche échoue, la pipe est cassée
	// mais on retourne le statut de la dernière commande
	if (WIFEXITED(status_right))
		return (WEXITSTATUS(status_right));
	else if (WIFSIGNALED(status_right))
		return (128 + WTERMSIG(status_right));
	return (1);
}

int	traverse_andor(t_ast *node, t_node_type type)
{
	int	res;

	res = -1;
	andor_propagate_fd(node);
	res = traverse_node_new(node->left);
	if (type == NODE_AND)
	{
		if (res == EXIT_SUCCESS)
			res = traverse_node_new(node->right);
	}
	else // NODE_OR
	{
		if (res != EXIT_SUCCESS)
			res = traverse_node_new(node->right);
	}
	return (res);
}

