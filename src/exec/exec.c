/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/04 16:25:40 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	exec_abs_path(t_ast *node)
{
	if (access(node->command->path, X_OK) == 0)
		return (0);
	perror("minishell");
	return (1);
}

static int	path_not_found(void)
{
	extern int	errno;

	errno = ENOENT;
	perror("minishell: no defined path");
	return (-1);
}

static int	exec_get_path(t_ast *node)
{
	char	**path_dirs;
	char	*path;
	int		i;

	if (*node->command->path == '.' || *node->command->path == '/')
		return (exec_abs_path(node));
	path = env_get(*node->env, "PATH");
	if (path == NULL)
		return (path_not_found());
	path_dirs = ft_split(path, ':');
	if (path_dirs == NULL)
		return (perror("minishell: split error in PATH"), -1);
	i = -1;
	while (path_dirs[++i])
	{
		path = ft_strjoin(path_dirs[i], "/");
		path = ft_strjoin_free_first(path, node->command->path);
		if (access(path, X_OK) == 0)
		{
			free(node->command->path);
			return (node->command->path = path, ft_free_tab(path_dirs), 0);
		}
		free(path);
	}
	return (ft_free_tab(path_dirs), perror("Error: program not in PATH"), -1);
}

void	init_process(t_ast *process, char **envp)
{
	if (exec_get_path(process) != 0)
	{
		ft_free_tab(envp);
		exit_and_free(process, EXIT_FAILURE, "exec: can't find file");
	}
	if (process->fd_in != STDIN_FILENO
		&& (dup2(process->fd_in, STDIN_FILENO) == -1
			|| close(process->fd_in) == -1))
	{
		ft_free_tab(envp);
		exit_and_free(process, EXIT_FAILURE, "exec: error duping fd_in");
	}
	if (process->fd_out != STDOUT_FILENO
		&& (dup2(process->fd_out, STDOUT_FILENO) == -1
			|| close(process->fd_out) == -1))
	{
		ft_free_tab(envp);
		exit_and_free(process, EXIT_FAILURE, "exec: error duping fd_out");
	}
}

int	exec_process(t_ast *process)
{
	pid_t	pid;
	int		return_value;
	char	**envp;

	return_value = -1;
	pid = fork();
	if (pid == -1)
		exit_and_free(process, EXIT_FAILURE, "exec: error forking process");
	if (pid == CHILD_PID)
	{
		envp = env_lst_to_str_array(*process->env);
		init_process(process, envp);
		execve(process->command->path, process->command->args, envp);
	}
	waitpid(pid, &return_value, 0);
	return (return_value);
}
