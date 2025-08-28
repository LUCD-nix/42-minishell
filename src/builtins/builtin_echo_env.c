/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo_env.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 12:28:55 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/27 10:43:22 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../minishell.h"
#include <stdlib.h>

int	builtin_echo(int argc, char **argv)
{
	int	i;
	int	write_new_line;

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
// int	main(void)
// {
// 	extern char **environ;
//
// 	t_list	*a = env_lst_from_str_arr(environ);
// 	// t_ast export = {
// 	// 	.type = NODE_BUILTIN,
// 	// 	.command = &(t_command){
// 	// 		.path = "export",
// 	// 		.args = (char *[4]) {"export", "TEST1=1", "TEST2=2",
// NULL},
// 	// 	},
// 	// 	.env = &a,
// 	// };
// 	// t_ast unset = {
// 	// 	.type = NODE_BUILTIN,
// 	// 	.command = &(t_command){
// 	// 		.path = "unset",
// 	// 		.args = (char *[4]) {"unset", "TEST1=1", "TEST2=2",
// NULL},
// 	// 	},
// 	// 	.env = &a,
// 	// };
// 	t_ast cd = {
// 		.type = NODE_BUILTIN,
// 		.command = &(t_command){
// 			.path = "cd",
// 			.args = (char *[3]) {"cd", "../", NULL},
// 		},
// 		.env = &a,
// 	};
// 	// t_ast export_empty = {
// 	// 	.type = NODE_BUILTIN,
// 	// 	.command = &(t_command){
// 	// 		.path = "export",
// 	// 		.args = (char *[2]) {"export", NULL},
// 	// 	},
// 	// 	.env = &a,
// 	// };
// 	ft_printf("=================================================\n");
// 	ft_printf("=================================================\n");
// 	ft_printf("\n\n");
// 	ft_printf("cd status = %d, OLDPWD = %s PWD = %s\n", builtin_cd(2, &cd),
// env_get(a, "OLDPWD"), env_get(a, "PWD"));
// 	ft_printf("=================================================\n");
// 	ft_printf("=================================================\n");
// 	ft_printf("\n\n");
// 	ft_printf("cd status = %d, OLDPWD = %s PWD = %s\n", builtin_cd(2, &cd),
// env_get(a, "OLDPWD"), env_get(a, "PWD")); 	ft_lstclear(&a, &env_free);
// return (0);
// }
