/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:28:55 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/11 12:28:57 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../minishell.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>

int	builtin_echo(int argc, char **argv, char **envp)
{
	int	i;
	int	write_new_line;

	(void) envp;
	if (argc == 1)
	{
		if (ft_printf("\n") < 0)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	i = 1;
	write_new_line = ft_strncmp(argv[1], "-n", 3);
	if (!write_new_line)
		i++;
	while (i < argc)
	{
		if (ft_printf("%s", argv[i]) < 0)
			return (EXIT_FAILURE);
		i++;
	}
	if (write_new_line)
	{
		if (ft_printf("\n") < 0)
			return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static int	get_home_dir(char *buf, t_list *env)
{
	char *res;

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
	static char	home[PATH_MAX] = { 0 };
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
int	builtin_pwd(int argc,  char **argv, char **envp)
{
	char	cwd_buffer[PATH_MAX];

	(void) argv;
	(void) envp;
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

int	builtin_env(int argc, char **argv, char **envp)
{
	int			i;

	(void) argv;
	i = 0;
	if (argc != 1)
	{
		ft_printf("env : No arguments or flags supported!\n");
		return (EXIT_FAILURE);
	}
	while (envp[i] != NULL)
	{
		if (ft_printf("%s\n", envp[i]) < 0)
			return (EXIT_FAILURE);
		i++;
	}
	return (EXIT_SUCCESS);
}

static char	*key_from_args(t_ast *node, int i)
{
	int		j;
	char	*key;
	char	**args;

	j = 0;
	args = node->command->args;
	while (args[i][j] && args[i][j] != '=')
		j++;
	key = ft_calloc(j + 1, 1);
	if (!key)
		return (NULL);
	ft_strlcpy(key, node->command->args[i], j + 1);
	return (key);
}

int	builtin_unset(int argc, t_ast *node)
{
	t_list **env_lst;
	char	*key;
	int		i;

	env_lst = node->env;
	i = 0;
	while (++i < argc)
	{
		key = key_from_args(node, i);
		if (key == NULL)
			return (EXIT_FAILURE);
		env_delete_key(env_lst, key);
		free(key);
	}
	return (EXIT_SUCCESS);
}

static int	export_no_args(t_ast *node)
{
	t_list	*cursor;
	t_env	temp;

	cursor = *node->env;
	while (cursor != NULL)
	{
		temp = *(t_env *)cursor->content;
		if (ft_printf("declare -x %s=\"%s\"\n", temp.key, temp.value) < 0)
			return (EXIT_FAILURE);
		cursor = cursor->next;
	}
	return (EXIT_SUCCESS);
}

int	builtin_export(int argc, t_ast *node)
{
	t_list	**env_lst;
	char	*key;
	int		i;

	if (argc == 1)
		return (export_no_args(node));
	env_lst = node->env;
	i = 0;
	while (++i < argc)
	{
		key = key_from_args(node, i);
		if (!key)
			return (EXIT_FAILURE);
		env_delete_key(env_lst, key);
		env_lst_add(env_lst, node->command->args[i]);
		free(key);
	}
	return (EXIT_SUCCESS);
}

int	main(void)
{
	extern char **environ;

	t_list	*a = env_lst_from_str_arr(environ);
	// t_ast export = {
	// 	.type = NODE_BUILTIN,
	// 	.command = &(t_command){
	// 		.path = "export",
	// 		.args = (char *[4]) {"export", "TEST1=1", "TEST2=2", NULL},
	// 	},
	// 	.env = &a,
	// };
	// t_ast unset = {
	// 	.type = NODE_BUILTIN,
	// 	.command = &(t_command){
	// 		.path = "unset",
	// 		.args = (char *[4]) {"unset", "TEST1=1", "TEST2=2", NULL},
	// 	},
	// 	.env = &a,
	// };
	t_ast cd = {
		.type = NODE_BUILTIN,
		.command = &(t_command){
			.path = "cd",
			.args = (char *[3]) {"cd", "../", NULL},
		},
		.env = &a,
	};
	// t_ast export_empty = {
	// 	.type = NODE_BUILTIN,
	// 	.command = &(t_command){
	// 		.path = "export",
	// 		.args = (char *[2]) {"export", NULL},
	// 	},
	// 	.env = &a,
	// };
	ft_printf("=================================================\n");
	ft_printf("=================================================\n");
	ft_printf("\n\n");
	ft_printf("cd status = %d, OLDPWD = %s PWD = %s\n", builtin_cd(2, &cd), env_get(a, "OLDPWD"), env_get(a, "PWD"));
	ft_printf("=================================================\n");
	ft_printf("=================================================\n");
	ft_printf("\n\n");
	ft_printf("cd status = %d, OLDPWD = %s PWD = %s\n", builtin_cd(2, &cd), env_get(a, "OLDPWD"), env_get(a, "PWD"));
	ft_lstclear(&a, &env_free);
	return (0);
}
