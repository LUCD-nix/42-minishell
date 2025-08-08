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

// TODO : replace all the return (-1) with appropriate error functions
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
		if (process->fd_in != STDIN_FILENO)
		{
			if (dup2(process->fd_in, STDIN_FILENO) == -1)
				return (-1);
		}
		if (process->fd_out != STDOUT_FILENO)
		{
			if (dup2(process->fd_out, STDOUT_FILENO) == -1)
				return (-1);
		}
		execve(process->command->path, process->command->args, environ);
	}
	else
		waitpid(pid, &return_value, 0);
	return (return_value);
}

void	pipe_propagate_fd(t_ast *node)
{
	node->left->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}

void	andor_propagate_fd(t_ast *node)
{
	node->left->fd_in = node->fd_in;
	node->left->fd_out = node->fd_out;
	node->right->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}

int	file_to_pipe(char *file)
{
	int		file_fd;
	int		pipe_fd[2];
	char	*line;

	file_fd = open(file, O_RDONLY);
	if (file_fd == -1)
		return (-1);
	if (pipe(pipe_fd) == -1)
		return (-1);
	while (1)
	{
		line = get_next_line(file_fd);
		if (line == NULL)
			break ;
		if (write(pipe_fd[PIPE_IN], line, ft_strlen(line)) == -1)
			return (-1);
		free(line);
	}
	if (close(file_fd) == -1 || close(pipe_fd[PIPE_IN]) == -1)
		return (-1);
	return (pipe_fd[PIPE_OUT]);
}

int	traverse(t_ast *node)
{
	int		res;
	int		file_fd;

	res = -1;
	if (node->type == NODE_CMD)
	{
		res = run_process(node);
	}
	else if (node->type == NODE_PIPE)
	{
		pipe_propagate_fd(node);
		if (create_pipe(node->left, node->right) == -1)
			return (-1);
		traverse(node->left);
		if (close(node->left->fd_out) == -1)
			return (-1);
		res = traverse(node->right);
		if (close(node->right->fd_in) == -1)
			return (-1);
	}
	// TODO : untested
	else if (node->type == NODE_IN)
	{
		andor_propagate_fd(node);
		file_fd = file_to_pipe(node->right->filename);
		if (file_fd == -1)
			return (-1);
		node->left->fd_in = file_fd;
		res = traverse(node->left);
		close(file_fd);
	}
	else if (node->type == NODE_OUT)
	{
		file_fd = open(node->right->filename,
			 O_WRONLY | O_TRUNC | O_CREAT, 0644);
		if (file_fd == -1)
			return (-1);
		node->left->fd_out = file_fd;
		res = traverse(node->left);
		close(file_fd);
	}
	else if (node->type == NODE_APPEND)
	{
		file_fd = open(node->right->filename,
			 O_WRONLY | O_APPEND | O_CREAT, 0644);
		if (file_fd == -1)
			return (-1);
		node->left->fd_out = file_fd;
		res = traverse(node->left);
		close(file_fd);
	}
	else if (node->type == NODE_AND)
	{
		andor_propagate_fd(node);
		res = traverse(node->left);
		if (res == EXIT_SUCCESS)
			res = traverse(node->right);
	}
	// TODO : untested
	else if (node->type == NODE_OR)
	{
		andor_propagate_fd(node);
		res = traverse(node->left);
		if (res != EXIT_SUCCESS)
			res = traverse(node->right);
	}
	return (res);
}

int main(void)
{
	// t_ast	cat = {
	// 	.type = NODE_CMD,
	// 	.command = &(t_command) {
	// 		.path = "/usr/bin/cat",
	// 		.args = (char *[3]) {"cat", "test.txt", NULL},
	// 	},
	// };
	t_ast	wcl = {
		.type = NODE_CMD,
		.command = &(t_command) {
			.path = "/usr/bin/wc",
			.args = (char *[3]) {"wc", "-l", NULL},
		},
	};
	// t_ast	wcc = {
	// 	.type = NODE_CMD,
	// 	.command = &(t_command) {
	// 		.path = "/usr/bin/wc",
	// 		.args = (char *[3]) {"wc", "-c", NULL},
	// 	},
	// };
	// t_ast	factor = {
	// 	.type = NODE_CMD,
	// 	.command = &(t_command) {
	// 		.path = "/usr/bin/factor",
	// 		.args = (char *[2]) {"factor", NULL},
	// 	},
	// };
	// t_ast	pipe3 = {
	// 	.type = NODE_PIPE,
	// 	.left = &cat,
	// 	.right = &wcc,
	// };
	// t_ast	pipe2 = {
	// 	.type = NODE_PIPE,
	// 	.left = &cat,
	// 	.right = &wcl,
	// };
	// t_ast	and = {
	// 	.type = NODE_AND,
	// 	.left = &pipe2,
	// 	.right = &pipe3,
	// };
	// t_ast	pipe1 = {
	// 	.type = NODE_PIPE,
	// 	.left = &and,
	// 	.right = &factor,
	// 	.fd_out = STDOUT_FILENO,
	// 	.fd_in = STDIN_FILENO,
	// };
	// t_ast	outfile = {
	// 	.type = NODE_FILE,
	// 	.filename = "test_output.txt",
	// };
	// t_ast	redirect_append = {
	// 	.type = NODE_APPEND,
	// 	.left = &pipe1,
	// 	.right = &outfile,
	// };
	// traverse(&redirect_append);
	t_ast	in_file = {
		.type = NODE_FILE,
		.filename = "test.txt",
	};
	t_ast	redir_in = {
		.type = NODE_IN,
		.left = &wcl,
		.right = &in_file,
		.fd_in = 0,
		.fd_out = 1,
	};
	traverse(&redir_in);
	return (0);
}
