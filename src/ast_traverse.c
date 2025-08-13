/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_traverse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/04 16:25:40 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../minishell.h"
#include <readline/readline.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
//
// int	close_and_redirect(int to_close, int to_redirect)
// {
// 	if (close(to_close) == -1)
// 		return (-1);
// 	if (dup2(to_redirect, to_close) == -1)
// 		return (-1);
// 	return (0);
// }
//
// int	redirect_file(t_redir *lst)
// {
// 	int		oflag;
// 	int		fd;
// 	int		io;
//
// 	while (lst)
// 	{
// 		if (lst->type == REDIR_APPEND)
// 		{
// 			oflag = O_APPEND | O_WRONLY | O_CREAT;
// 			io = STDOUT_FILENO;
// 		}
// 		else if (lst->type == REDIR_OUT)
// 		{
// 			oflag = O_TRUNC | O_WRONLY | O_CREAT;
// 			io = STDOUT_FILENO;
// 		}
// 		else if (lst->type == REDIR_IN)
// 		{
// 			oflag = O_RDONLY;
// 			io = STDIN_FILENO;
// 		}
// 		fd = open(lst->filename, oflag);
// 		if (fd == -1)
// 			return (-1);
// 		if (close_and_redirect(io, fd) == -1)
// 			return (-1);
// 		lst = lst->next;
// 	}
// 	return (0);
// }
//
// int	run_process(t_ast *process)
// {
// 	pid_t		pid;
// 	int			return_value;
// 	extern char	*environ[];
//
// 	if (redirect_file(process->redir) == -1)
// 		;// TODO: error out
// 	return_value = 0;
// 	pid = fork();
// 	if (pid == -1)
// 		;// TODO : Error out
// 	if (pid == CHILD_PID)
// 	{
// 		execve(process->path, process->args, environ);
// 	}
// 	else
// 		waitpid(pid, &return_value, 0);
// 	return (return_value);
// }
//
// int	traverse(t_ast *node)
// {
// 	int		pepi[2];
// 	int		res;
//
// 	if (node->type == CMD_SIMPLE)
// 	{
// 		return (run_process(node));
// 	}
// 	else if (node->type == CMD_PIPE)
// 	{
// 		if (pipe(pepi) == -1)
// 			; // TODO: exit;
// 		if (close_and_redirect(STDOUT_FILENO, pepi[PIPE_IN]) == -1 ||
// 			close_and_redirect(STDIN_FILENO, pepi[PIPE_OUT]) == -1)
// 			return (-1);
// 		traverse(node->left);
// 		close(pepi[PIPE_OUT]);
// 		close(STDOUT_FILENO);
// 		res = traverse(node->right);
// 		close(pepi[PIPE_IN]);
// 		return (res);
// 	}
// 	// La deuxieme commande ne s'execute que si
// 	// la premiere a termine correctement
// 	else if (node->type == CMD_AND)
// 	{
// 		res = traverse(node->left);
// 		if (res == EXIT_FAILURE)
// 			return res;
// 		else
// 			return (traverse(node->right));
// 	}
// 	// La deuxieme commande ne s'execute que si
// 	// la premiere a termine avec une erreur
// 	else
// 	{
// 		res = traverse(node->left);
// 		if (res == EXIT_SUCCESS)
// 			return (res);
// 		else
// 			return (traverse(node->right));
// 	}
// }
//
// int main(void)
// {
// 	t_ast	ls = {
// 		.type = CMD_SIMPLE,
// 		.path = "/usr/bin/ls",
// 		.args = (char *[3]) {"ls", "-la", NULL},
// 	};
// 	t_ast	wc = {
// 		.type = CMD_SIMPLE,
// 		.path = "/usr/bin/wc",
// 		.args = (char *[3]) {"wc", "-l", NULL},
// 	};
// 	t_ast	pipe = {
// 		.type = CMD_PIPE,
// 		.left = &ls,
// 		.right = &wc
// 	};
// 	traverse(&pipe);
// 	return (0);
// }
