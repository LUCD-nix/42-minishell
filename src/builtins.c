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
//
// TODO : builtin_* should all return EXIT_SUCCESS EXIT_FAILURE
//
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

static int	get_home_dir(char	*buf)
{
	char *res;

	res = getenv("HOME");
	if (res == NULL)
	{
		return (1);
	}
	ft_strlcpy(buf, res, PATH_MAX);
	return (0);
}

int	builtin_cd(int argc, char **argv, char **envp)
{
	static char	home[PATH_MAX] = { 0 };

	// TODO : handle cd - to go back to previous location
	(void) envp;
	if (!*home)
	{
		if (get_home_dir(home) == 1)
			return (-1);
	}
	if (argc > 2)
	{
		// TODO : implement print to stderror
		ft_printf("cd : Too many arguments");
		return (-1);
	}
	if (argc == 1)
		return (chdir(home));
	else
		return (chdir(argv[1]));
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

char	*key_from_args(t_ast *node, int i)
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
	t_env	*env_array;
	char	*key;
	int		i;

	env_array = node->env;
	i = 0;
	while (++i < argc)
	{
		key = key_from_args(node, i);
		if (key == NULL)
			return (EXIT_FAILURE);
		env_remove_key(env_array, key);
		free(key);
	}
	return (EXIT_SUCCESS);
}

int	builtin_export(int argc, t_ast *node)
{
	t_env	*env_array;
	char	*key;
	int		i;

	// TODO : 
	// if (argc == 1)
	// 	return (export_no_args(envp));
	env_array = node->env;
	i = 0;
	while (++i < argc)
	{
		key = key_from_args(node, i);
		if (!key)
			return (EXIT_FAILURE);
		env_remove_key(env_array, key);
		env_add(env_array, node->command->args[i]);
		free(key);
	}
	return (EXIT_SUCCESS);
}

int	main(void)
{
	extern char **environ;

	t_env *a = env_from_str_arr(environ);
	t_ast export = {
		.type = NODE_BUILTIN,
		.command = &(t_command){
			.path = "export",
			.args = (char *[4]) {"export", "TEST1=1", "TEST2=2", NULL},
		},
		.env = a,
	};
	t_ast unset = {
		.type = NODE_BUILTIN,
		.command = &(t_command){
			.path = "unset",
			.args = (char *[4]) {"unset", "TEST1=1", "TEST2=2", NULL},
		},
		.env = a,
	};
	builtin_export(3, &export);
	builtin_env(1, NULL, a->contents);
	builtin_unset(3, &unset);
	builtin_env(1, NULL, a->contents);
	free(a);
	return (0);
}
