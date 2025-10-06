/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/10/02 13:39:42 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	exec_get_path(t_ast *node)
{
	char	**path_dirs;
	char	*path;

	if (*node->command->path == '\0')
		return (path_not_found());
	if (*node->command->path == '.' || *node->command->path == '/')
		return (exec_abs_path(node));
	path = env_get(*node->env, "PATH");
	if (path == NULL)
		return (path_not_found());
	path_dirs = ft_split(path, ':');
	if (path_dirs == NULL)
		return (perror("minishell: split error in PATH"), -1);
	return (exec_get_path_loop(path_dirs, node));
}

void	init_process(t_ast *process, char **envp)
{
	if (exec_get_path(process) != 0)
	{
		ft_free_tab(envp);
		exit_and_free(process, EXIT_FAILURE, "exec: can't find file");
	}
	if (process->fd_in != STDIN_FILENO && (dup2(process->fd_in, STDIN_FILENO)
			== -1 || close(process->fd_in) == -1))
	{
		ft_free_tab(envp);
		exit_and_free(process, EXIT_FAILURE, "exec: error duping fd_in");
	}
	if (process->fd_out != STDOUT_FILENO && (dup2(process->fd_out,
				STDOUT_FILENO) == -1 || close(process->fd_out) == -1))
	{
		ft_free_tab(envp);
		exit_and_free(process, EXIT_FAILURE, "exec: error duping fd_out");
	}
}

static int	handle_signal_exit(int return_value)
{
	if (WIFSIGNALED(return_value))
	{
		if (WTERMSIG(return_value) == SIGINT)
		{
			write(STDOUT_FILENO, "\n", 1);
			return (130);
		}
		if (WTERMSIG(return_value) == SIGQUIT)
		{
			write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
			return (131);
		}
	}
	return (WEXITSTATUS(return_value));
}

static void	exec_child_process(t_ast *process)
{
	char	**envp;
	int		exec_result;

	setup_child_signals();
	envp = env_lst_to_str_array(*process->env);
	init_process(process, envp);
	exec_result = execve(process->command->path, process->command->args, envp);
	if (exec_result == -1)
	{
		ft_free_tab(envp);
		exit_and_free(process, EXIT_FAILURE, "execve failed");
	}
}

int	exec_process(t_ast *process)
{
	pid_t	pid;
	int		return_value;

	return_value = -1;
	pid = fork();
	if (pid == -1)
		exit_and_free(process, EXIT_FAILURE, "exec: error forking process");
	if (pid == CHILD_PID)
		exec_child_process(process);
	ignore_signals();
	waitpid(pid, &return_value, 0);
	setup_interactive_signals();
	return (handle_signal_exit(return_value));
}
