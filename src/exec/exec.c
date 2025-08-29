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

// void	free_envp(char **envp)
// {
// // TODO : this is also needed to free args in command struct, should reuse
// 	int	i;
//
// 	i = -1;
// 	while (envp[++i] != NULL)
// 		free(envp[i]);
// 	free(envp);
// }
//
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
	// TODO :
	// if (!ft_memcmp(node->command->path, "exit", 5))
	// 	return(builtin_exit(argc, argv, envp));
	ft_free_tab(envp);
	return (res);;
}

int	exec_process(t_ast *process)
{
	pid_t		pid;
	int			return_value;
	char		**envp;

	return_value = 0;
	pid = fork();
	if (pid == -1)
		return (-1);
	envp = env_lst_to_str_array(*process->env);
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
	{
		waitpid(pid, &return_value, 0);
		while (!WIFEXITED(return_value) && !WIFSIGNALED(return_value))
			waitpid(pid, &return_value, 0);
		ft_free_tab(envp);
	}
	return (return_value);
}
