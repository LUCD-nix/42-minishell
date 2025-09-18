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
#include <stdlib.h>

int	exec_builtin(t_ast *node)
{
	int		argc;
	int		res;
	char	**argv;
	char	**envp;

	res = -1;
	argc = 0;
	argv = node->command->args;
	envp = env_lst_to_str_array(*node->env);
	while (argv[argc] != NULL)
		argc++;
	if (!ft_memcmp(node->command->path, "echo", 5))
		res = builtin_echo(argc, argv);
	else if (!ft_memcmp(node->command->path, "cd", 3))
		res = builtin_cd(argc, node);
	else if (!ft_memcmp(node->command->path, "pwd", 4))
		res = builtin_pwd(argc);
	else if (!ft_memcmp(node->command->path, "export", 7))
		res = builtin_export(argc, node);
	else if (!ft_memcmp(node->command->path, "unset", 6))
		res = builtin_unset(argc, node);
	else if (!ft_memcmp(node->command->path, "env", 4))
		res = (builtin_env(argc, argv, envp));
	if (!ft_memcmp(node->command->path, "exit", 5))
		return (builtin_exit(node, argc, argv, envp));
	ft_free_tab(envp);
	return (res);
}

static int	exec_get_path(t_ast *node)
{
	char	**path_dirs;
	char	*path;
	char	*path_to_elf;
	int		i;

	path = env_get(*node->env, "PATH");
	if (path == NULL)
		return (perror("minishell: no defined PATH\n"), -1);
	path_dirs = ft_split(path, ':');
	if (path_dirs == NULL)
		return (perror("minishell: split error in PATH\n"), -1);
	i = -1;
	while (path_dirs[++i])
	{
		path_to_elf = ft_strjoin(path_dirs[i], "/");
		path_to_elf = ft_strjoin_free_first(path_to_elf, node->command->path);
		if (access(path_to_elf, X_OK) == 0)
		{
			free(node->command->path);
			node->command->path = path_to_elf;
			return (ft_free_tab(path_dirs), 0);
		}
		free(path_to_elf);
	}
	return (ft_free_tab(path_dirs), perror("Error: program not in PATH"), -1);
}

int	exec_process(t_ast *process)
{
	pid_t		pid;
	int			return_value;
	char		**envp;

	return_value = -1;
	pid = fork();
	if (pid == -1)
		exit_and_free(process, EXIT_FAILURE, "exec: error forking process");
	envp = env_lst_to_str_array(*process->env);
	if (pid == CHILD_PID)
	{
		if ((exec_get_path(process) == -1)
			|| (process->fd_in != STDIN_FILENO
				&& dup2(process->fd_in, STDIN_FILENO) == -1)
			|| (process->fd_out != STDOUT_FILENO
				&& dup2(process->fd_out, STDOUT_FILENO) == -1))
			return (ft_free_tab(envp),
				exit_and_free(process, EXIT_FAILURE, "exec: Error setting up"),
				EXIT_FAILURE);
		execve(process->command->path, process->command->args, envp);
	}
	waitpid(pid, &return_value, 0);
	ft_free_tab(envp);
	return (return_value);
}
