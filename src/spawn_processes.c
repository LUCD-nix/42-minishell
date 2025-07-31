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
		; // TODO
	if (pid == CHILD_PID)
	{
		if (process->in != STDIN && dup2(process->fd_in, STDIN) == -1)
			; // TODO
		if (process->out != STDOUT && dup2(process->fd_out, STDOUT) == -1)
			; // TODO
		execve(process->command, process->args, environ);
		// ^^ environ is probably what we want but idk
	}
	else
	{
		// WCONTINUED might not be necessary here i dont quite get what it does
		waitpid(pid, &exit_status, WCONTINUED);
	}
	return (exit_status);
}
