/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_pwd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 10:28:17 by lucorrei          #+#    #+#             */
/*   Updated: 2025/10/03 17:16:40 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <asm-generic/errno-base.h>

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
	int	res;

	if (env_set(env, "OLDPWD", getcwd(NULL, 0)) == -1)
	{
		env_lst_add(&env, "OLDPWD");
		env_set(env, "OLDPWD", getcwd(NULL, 0));
	}
	res = chdir(to_change);
	if (res == 0)
		env_set(env, "PWD", getcwd(NULL, 0));
	return (res);
}

static int	cd_dash(t_list *env)
{
	int		res;
	char	*old;
	char	*current;

	old = env_get(env, "OLDPWD");
	current = ft_strdup(env_get(env, "PWD"));
	res = chdir(old);
	if (res == 0)
	{
		env_set(env, "PWD", getcwd(NULL, 0));
		env_set(env, "OLDPWD", current);
		return (0);
	}
	return (1);
}

int	builtin_cd(int argc, t_ast *node)
{
	static char	home[PATH_MAX] = {0};
	t_list		**env;
	extern int	errno;

	env = node->env;
	if (!*home)
		get_home_dir(home, *env);
	if (argc > 2)
		return (errno = E2BIG, perror("minishell: cd"), -1);
	if (argc == 1)
	{
		if (chdir(home) == -1)
			return (errno = 2, perror("cd: HOME not set"), -1);
		return (EXIT_SUCCESS);
	}
	else
	{
		if (ft_memcmp("-", node->command->args[1], 2) == 0)
		{
			if (env_get(*env, "OLDPWD") == NULL)
				return (ft_printf("cd : OLDPWD not set\n"), EXIT_FAILURE);
			return (cd_dash(*env));
		}
		return (chdir_wrapper(node->command->args[1], *env));
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
