/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_pwd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 10:28:17 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/27 10:28:21 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static int	get_home_dir(char *buf, t_list *env)
{
	char	*res;

	res = env_get(env, "HOME");
	if (res == NULL)
		return (1);
	ft_strlcpy(buf, res, PATH_MAX);
	return (0);
}

static int	chdir_wrapper(char *to_change, t_list *env)
{
	t_list	*cursor;
	int		res;

	cursor = env;
	env_set(env, "OLDPWD", getcwd(NULL, 0));
	res = chdir(to_change);
	env_set(env, "PWD", getcwd(NULL, 0));
	return (res);
}

int	builtin_cd(int argc, t_ast *node)
{
	static char	home[PATH_MAX] = {0};
	t_list		*env;

	env = *node->env;
	if (!*home && get_home_dir(home, env) == 1)
		return (EXIT_FAILURE);
	if (argc > 2)
	{
		// TODO : implement print to stderror
		ft_printf("cd : Too many arguments");
		return (-1);
	}
	if (argc == 1)
		return (chdir(home));
	else
	{
		if (ft_memcmp("-", node->command->args[1], 2) == 0)
		{
			if (env_get(env, "OLDPWD") == NULL)
				return (ft_printf("cd : OLDPWD not set\n"), EXIT_FAILURE);
			return (chdir_wrapper(env_get(env, "OLDPWD"), env));
		}
		return (chdir_wrapper(node->command->args[1], env));
	}
}

// PATH_MAX is 4096 according to getconf PATH_MAX,
// maybe get it from env right here instead of
// harcoding it
int	builtin_pwd(int argc)
{
	char	cwd_buffer[PATH_MAX];

	if (argc != 1)
	{
		ft_printf("pwd : too many arguments\n");
		return (EXIT_FAILURE);
	}
	if (getcwd(cwd_buffer, PATH_MAX) == NULL)
	{
		return (EXIT_FAILURE);
	}
	if (ft_printf("%s\n", cwd_buffer) < 0)
	{
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
