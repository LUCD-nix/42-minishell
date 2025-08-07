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
			&& dup2(process->fd_in, STDIN_FILENO) == -1)
				return (-1);
		if (process->fd_out != STDOUT_FILENO
			&& dup2(process->fd_out, STDOUT_FILENO) == -1)
				return (-1);
		execve(process->command->path, process->command->args, environ);
	}
	else
		waitpid(pid, &return_value, 0);
	return (return_value);
}

void	pipe_propagate_fd(t_ast *pipe)
{
	pipe->left->fd_in = pipe->fd_in;
	pipe->right->fd_out = pipe->fd_out;
}

int	traverse(t_ast *node)
{
	int		res;

	res = -1;
	if (node->type == NODE_CMD)
	{
		res = run_process(node);
	}
	else if (node->type == NODE_PIPE)
	{
		pipe_propagate_fd(node);
		if (create_pipe(node->left, node->right) == -1)
			res = -1;
		else
		{
			traverse(node->left);
			res = traverse(node->right);
		}
	}
	if (node->fd_in != STDIN_FILENO)
		close(node->fd_in);
	if (node->fd_out != STDOUT_FILENO)
		close(node->fd_out);
	return (res);
}
// 	// La deuxieme commande ne s'execute que si
// 	// la premiere a termine correctement
// 	else if (node->type == NODE_AND)
// 	{
// 		res = traverse(node->left);
// 		if (res == EXIT_SUCCESS)
// 			res = traverse(node->right);
// 	}
// 	// La deuxieme commande ne s'execute que si
// 	// la premiere a termine avec une erreur
// 	else if (node->type == NODE_OR)
// 	{
// 		res = traverse(node->left);
// 		if (res != EXIT_SUCCESS)
// 			res = traverse(node->right);
// 	}

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
	t_ast	wc2 = {
		.type = NODE_CMD,
		.command = &(t_command) {
			.path = "/usr/bin/wc",
			.args = (char *[3]) {"wc", "-c", NULL},
		},
		.fd_out = STDOUT_FILENO,
		.fd_in = STDIN_FILENO,
	};
	t_ast	factor = {
		.type = NODE_CMD,
		.command = &(t_command) {
			.path = "/usr/bin/factor",
			.args = (char *[2]) {"factor", NULL},
		},
		.fd_out = STDOUT_FILENO,
		.fd_in = STDIN_FILENO,
	};
	t_ast	pipe3 = {
		.type = NODE_PIPE,
		.left = &factor,
		.right = &wc2,
		.fd_out = STDOUT_FILENO,
		.fd_in = STDIN_FILENO,
	};
	t_ast	pipe2 = {
		.type = NODE_PIPE,
		.left = &wc,
		.right = &pipe3,
		.fd_out = STDOUT_FILENO,
		.fd_in = STDIN_FILENO,
	};
	t_ast	pipe1 = {
		.type = NODE_PIPE,
		.left = &ls,
		.right = &pipe2,
		.fd_out = STDOUT_FILENO,
		.fd_in = STDIN_FILENO,
	};
	traverse(&pipe1);
	return (0);
}
