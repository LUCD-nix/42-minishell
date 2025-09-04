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
	else if (!ft_memcmp(node->command->path, "exit", 5))
		res = builtin_exit(argc, argv, node->env);
	ft_free_tab(envp);
	return (res);
}

static int	exec_get_path(t_ast *node)
{
	char	**path_dirs;
	char	*path;
	char	*path_to_elf;
	int		i;

	// NOUVEAU: Vérifier si le path est vide
	if (!node->command->path || ft_strlen(node->command->path) == 0)
	{
		// Ne pas afficher d'erreur pour une commande vide
		return (-1);
	}

	// Si le chemin contient '/', ne pas utiliser PATH
	if (ft_strchr(node->command->path, '/'))
	{
		if (access(node->command->path, X_OK) == 0)
			return (0);
		perror("minishell");
		return (-1);
	}

	path = env_get(*node->env, "PATH");
	if (path == NULL)
	{
		ft_printf("minishell: %s: No such file or directory\n", node->command->path);
		return (-1);
	}
	
	path_dirs = ft_split(path, ':');
	if (path_dirs == NULL)
		return (-1);
	
	i = -1;
	while (path_dirs[++i])
	{
		path_to_elf = ft_strjoin(path_dirs[i], "/");
		if (!path_to_elf)
		{
			ft_free_tab(path_dirs);
			return (-1);
		}
		path_to_elf = ft_strjoin_free_first(path_to_elf, node->command->path);
		if (!path_to_elf)
		{
			ft_free_tab(path_dirs);
			return (-1);
		}
		if (access(path_to_elf, X_OK) == 0)
		{
			free(node->command->path);
			node->command->path = path_to_elf;
			ft_free_tab(path_dirs);
			return (0);
		}
		free(path_to_elf);
	}
	ft_free_tab(path_dirs);
	ft_printf("minishell: %s: command not found\n", node->command->path);
	return (-1);
}

int	exec_process(t_ast *process)
{
	pid_t		pid;
	int			return_value;
	char		**envp;

	// NOUVEAU: Vérifier si la commande est vide après expansion
	if (!process->command || !process->command->path || 
		ft_strlen(process->command->path) == 0)
	{
		// Bash retourne 0 pour une commande vide
		return (0);
	}

	return_value = -1;
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == CHILD_PID)
	{
		envp = env_lst_to_str_array(*process->env);
		if (!envp)
			exit(1);
		if (exec_get_path(process) == -1)
		{
			ft_free_tab(envp);
			exit(127);  // Command not found
		}
		if (process->fd_in != STDIN_FILENO
			&& dup2(process->fd_in, STDIN_FILENO) == -1)
		{
			ft_free_tab(envp);
			exit(1);
		}
		if (process->fd_out != STDOUT_FILENO
			&& dup2(process->fd_out, STDOUT_FILENO) == -1)
		{
			ft_free_tab(envp);
			exit(1);
		}
		execve(process->command->path, process->command->args, envp);
		perror("execve");
		ft_free_tab(envp);
		exit(126);  // Command invoked cannot execute
	}
	waitpid(pid, &return_value, 0);
	if (WIFEXITED(return_value))
		return (WEXITSTATUS(return_value));
	return (-1);
}
