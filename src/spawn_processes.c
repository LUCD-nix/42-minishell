/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawn_processes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 15:20:43 by lucorrei          #+#    #+#             */
/*   Updated: 2025/07/31 15:20:44 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../minishell.h"

int	init_pipe(t_command *writer, t_command *reader)
{
	int		pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (-1);
	reader->fd_in  = pipe_fd[0];
	writer->fd_out = pipe_fd[1];
	return (0);
}

int	run_process(t_command *process)
{
	pid_t		pid;
	int			exit_status;
	extern char	*environ[];

	exit_status = 0;
	if (process->type == E_BUILTIN)
	{
		// TODO
	}
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == CHILD_PID)
	{
		if (process->in != I_STDIN && dup2(process->fd_in, STDIN) == -1)
			; // TODO : error
		if (process->out != O_STDOUT && dup2(process->fd_out, STDOUT) == -1)
			; // TODO : error
		execve(process->command, process->args, environ);
		// ^^ should probably change environ here
	}
	else
	{
		waitpid(pid, &exit_status, 0);
		if (process->fd_in != I_STDIN)
			close(process->fd_in);
		if (process->fd_out != O_STDOUT)
			close(process->fd_out);
	}
	return (exit_status);
}

int	spawn_and_pipe(t_command *process1, t_command *process2)
{
	init_pipe(process1, process2);
	int status1 = run_process(process1);
	printf("status 1 = %d\n", status1);
	int status2 = run_process(process2);
	printf("status 2 = %d\n", status2);
	return (0);
}

int	main(void)
{
	t_command ls = {
		.command = "/usr/bin/ls",
		.args = (char *[2]){"ls", "-la",},
		.in = I_STDIN,
		.out = O_PIPE
	};
	t_command wc = {
		.command = "/usr/bin/wc",
		.args = (char *[2]){"wc", "-l",},
		.in = I_PIPE,
		.out = O_STDOUT
	};
	spawn_and_pipe(&ls, &wc);
	return (0);
}
