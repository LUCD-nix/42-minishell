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

int	init_pipe(t_command *process1, t_command *process2)
{
	int		pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (-1);
	process1->fd_out = pipe_fd[1];
	process2->fd_in  = pipe_fd[0];
	return (0);
}

int	start_process(t_command *process)
{
	pid_t		pid;
	int			exit_status;
	extern char	*environ[];

	if (process->type == E_BUILTIN)
	{
		// TODO
	}
	pid = fork();
	if (pid == -1)
		; // TODO : error
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
	}
	return (exit_status);
}
