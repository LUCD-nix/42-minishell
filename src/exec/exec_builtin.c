/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/04 16:25:40 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	exec_builtin2(t_ast *node, int argc, char **argv, char **envp)
{
	int	res;

	res = -1;
	if (!ft_memcmp(node->command->path, "unset", 6))
		res = builtin_unset(argc, node);
	else if (!ft_memcmp(node->command->path, "env", 4))
		res = (builtin_env(argc, argv, envp));
	if (!ft_memcmp(node->command->path, "exit", 5))
		return (builtin_exit(node, argc, argv, envp));
	return (res);
}

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
	else
		res = exec_builtin2(node, argc, argv, envp);
	ft_free_tab(envp);
	return (res);
}
