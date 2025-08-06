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

// TODO : replace all the return (-1) with appropriate error functions
//
int	create_pipe(t_ast *writer, t_ast *reader)
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

// int	close_and_redirect_fd(int to_close, int to_redirect)
// {
// 	if (close(to_close) == -1)
// 		return (-1);
// 	if (dup2(to_close, to_redirect) == -1)
// 		return (-1);
// 	return(0);
// }

int	run_process(t_ast *process)
{
	pid_t		pid;
	int			return_value;
	extern char	*environ[];

	return_value = -1;
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == CHILD_PID)
	{
		if (process->fd_in != STDIN_FILENO
			// && close_and_redirect_fd(STDIN_FILENO, process->fd_in) == -1)
			&& dup2(process->fd_in, STDIN_FILENO) == -1)
			return (-1);
		if (process->fd_out != STDOUT_FILENO
			&& dup2(process->fd_out, STDOUT_FILENO) == -1)
			return (-1);
		execve(process->command->path, process->command->args, environ);
	}
	else
	{
		waitpid(pid, &return_value, 0);
		if (process->fd_in != STDIN_FILENO)
			close(process->fd_in);
		if (process->fd_out != STDOUT_FILENO)
			close(process->fd_out);
	}
	return (return_value);
}

int	traverse(t_ast *node)
{
	int		res;

	if (node->type == NODE_CMD)
	{
		return (run_process(node));
	}
	else if (node->type == NODE_PIPE)
	{
		if (create_pipe(node->left, node->right) == -1)
			return (-1);
		traverse(node->left);
		return (traverse(node->right));
	}
	// La deuxieme commande ne s'execute que si
	// la premiere a termine correctement
	else if (node->type == NODE_AND)
	{
		res = traverse(node->left);
		if (res != EXIT_SUCCESS)
			return res;
		else
			return (traverse(node->right));
	}
	// La deuxieme commande ne s'execute que si
	// la premiere a termine avec une erreur
	else if (node->type == NODE_OR)
	{
		res = traverse(node->left);
		if (res == EXIT_SUCCESS)
			return (res);
		else
			return (traverse(node->right));
	}
	return (0);
}

int main(void)
{
	t_ast	ls = {
		.type = NODE_CMD,
		.command = &(t_command) {
			.path = "/usr/bin/ls",
			.args = (char *[3]) {"ls", "-la", NULL},
		},
		.fd_out = STDOUT_FILENO,
		.fd_in = STDIN_FILENO,
	};
	t_ast	wc = {
		.type = NODE_CMD,
		.command = &(t_command) {
			.path = "/usr/bin/wc",
			.args = (char *[3]) {"wc", "-l", NULL},
		},
		.fd_out = STDOUT_FILENO,
		.fd_in = STDIN_FILENO,
	};
	t_ast	pipe = {
		.type = NODE_PIPE,
		.left = &ls,
		.right = &wc
	};
	traverse(&pipe);
	return (0);
}
