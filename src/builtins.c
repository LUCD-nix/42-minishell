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
//
// TODO : might want to change return values in all these functions
//
int	builtin_echo(int argc, char **argv)
{
	int	i;
	int	write_new_line;

	if (argc == 1)
	{
		if (ft_printf("\n") < 0)
			return (-1);
		return (0);
	}
	i = 1;
	write_new_line = ft_strncmp(argv[1], "-n", 3);
	if (!write_new_line)
		i++;
	while (i < argc)
	{
		if (ft_printf("%s", argv[i]) < 0)
			return (-1);
		i++;
	}
	if (write_new_line)
	{
		if (ft_printf("\n") < 0)
			return (-1);
	}
	return (0);
}

// PATH_MAX is 4096 according to getconf PATH_MAX,
// maybe get it from env right here instead of
// harcoding it
int	builtin_pwd(int argc, char **argv)
{
	char	*cwd_buffer;

	(void) argv;
	if (argc != 1)
	{
		ft_printf("pwd : too many arguments\n");
		return (1);
	}
	cwd_buffer = NULL;
	cwd_buffer = ft_calloc(PATH_MAX, sizeof *cwd_buffer);
	if (cwd_buffer == NULL)
	{
		return (-1);
	}
	if (getcwd(cwd_buffer, PATH_MAX) == NULL)
	{
		free(cwd_buffer);
		return (-1);
	}
	if (ft_printf("%s\n", cwd_buffer) < 0)
	{
		free(cwd_buffer);
		return (-1);
	}
	free(cwd_buffer);
	return (0);
}

int	builtin_env(int argc, char **argv)
{
	extern char **environ;
	int			i = 0;

	(void) argv;
	if (argc != 1)
	{
		ft_printf("env : No arguments or flags supported!\n");
		return (1);
	}
	while (environ[i] != NULL)
	{
		if (ft_printf("%s\n", environ[i]) < 0)
			return (-1);
		i++;
	}
	return (0);
}
