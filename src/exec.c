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
#include "../minishell.h"

int	exec_builtin(t_ast *node)
{
	int		argc;
	char	**argv;
	char	**envp;

	argc = 0;
	argv = node->command->args;
	envp = node->env->contents;
	while (argv[argc] != NULL)
		argc++;
	if (!ft_memcmp(node->command->path, "echo", 5))
		return (builtin_echo(argc, argv,envp));
	if (!ft_memcmp(node->command->path, "cd", 3))
		return(builtin_cd(argc, argv, envp));
	if (!ft_memcmp(node->command->path, "pwd", 4))
		return (builtin_pwd(argc, argv, envp));
	if (!ft_memcmp(node->command->path, "export", 7))
		return(builtin_export(argc, node));
	if (!ft_memcmp(node->command->path, "unset", 6))
		return(builtin_unset(argc, node));
	if (!ft_memcmp(node->command->path, "env", 4))
		return (builtin_env(argc, argv, envp));
	// if (!ft_memcmp(node->command->path, "exit", 5))
	// 	return(builtin_exit(argc, argv, envp));
	// Unreachable
	return (-1);
}

int	exec_process(t_ast *process)
{
	pid_t		pid;
	int			return_value;
	char		**envp;

	return_value = -1;
	pid = fork();
	envp = process->env->contents;
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
		execve(process->command->path, process->command->args, envp);
	}
	else
		waitpid(pid, &return_value, 0);
	return (return_value);
}
