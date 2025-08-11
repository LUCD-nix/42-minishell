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
#include <unistd.h>

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
	extern char	**environ;

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

// TODO: (a | b && c | d) < file does not propagate file to "c"
// in BASH, and it does here, needs more testing
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

int	redirect_builtin(t_ast *node, int *saved_in, int *saved_out)
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

int	exec_builtin(t_ast *node)
{
	int 	argc;
	char	**argv;

	argc = 0;
	argv = node->command->args;
	while (argv[argc] != NULL)
		argc++;
	if (!ft_memcmp(node->command->path, "echo", 5))
		return (builtin_echo(argc, argv));
	// if (!ft_memcmp(node->command->path, "cd", 3))
	// 	return(builtin_cd(argc, argv));
	if (!ft_memcmp(node->command->path, "pwd", 4))
		return(builtin_pwd(argc, argv));
	// if (!ft_memcmp(node->command->path, "export", 7))
	// 	return(builtin_export(argc, argv));
	// if (!ft_memcmp(node->command->path, "unset", 6))
	// 	return(builtin_unset(argc, argv));
	if (!ft_memcmp(node->command->path, "env", 4))
		return(builtin_env(argc, argv));
	// if (!ft_memcmp(node->command->path, "exit", 5))
	// 	return(builtin_exit(argc, argv));
	// Unreachable
	return (-1);
}

int	handle_builtin(t_ast *node)
{
	int	saved_in;
	int	saved_out;
	int	res;

	saved_in = 0;
	saved_out = 0;
	if (redirect_builtin(node, &saved_in, &saved_out) == -1)
		return (-1);
	res = exec_builtin(node);
	if (node->fd_in != STDIN_FILENO)
	{
		if (dup2(saved_in, STDIN_FILENO) == -1)
			return (-1);
	}
	if (node->fd_out != STDOUT_FILENO)
	{
		if (dup2(saved_out, STDOUT_FILENO) == -1)
			return (-1);
	}
	return (res);
}

int	traverse(t_ast *node)
{
	int		res;
	int		file_fd;

	res = -1;
	if (node->type == NODE_CMD)
		res = run_process(node);
	else if (node->type == NODE_BUILTIN)
		res = handle_builtin(node);
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
	t_ast	wcc = {
		.type = NODE_CMD,
		.command = &(t_command) {
			.path = "/usr/bin/wc",
			.args = (char *[3]) {"wc", "-c", NULL},
		},
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
	};
	t_ast	echo = {
		.type = NODE_BUILTIN,
		.command = &(t_command) {
			.path = "echo",
			.args = (char *[5]) {"echo", "asdfas", "asdfasdf", "asdf asdf asfd", NULL}
		},
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
	};
	t_ast	pipe = {
		.type = NODE_PIPE,
		.left = &echo,
		.right = &wcc,
		.fd_in = STDIN_FILENO,
		.fd_out = STDOUT_FILENO,
	};
	return (traverse(&pipe));
}
