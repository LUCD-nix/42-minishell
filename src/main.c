/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:00:00 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 15:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	g_signal_received = 0;

char	*read_user_input(void)
{
	char	*line;

	line = readline("minishell$ ");
	if (!line)
	{
		printf("exit\n");
		return (NULL);
	}
	return (line);
}

void	handle_history(char *line)
{
	if (ft_strlen(line) > 0)
		add_history(line);
}

void	cleanup_and_exit(t_list **my_envp, int last_status)
{
	ft_lstclear(my_envp, &env_free);
	rl_clear_history();
	exit(last_status);
}

void	main_loop(t_list **my_envp, int *last_status)
{
	char	*line;
	int		continue_loop;

	while (1)
	{
		if (g_signal_received == SIGINT)
		{
			g_signal_received = 0;
			*last_status = 130;
		}
		handle_interactive_signals();
		line = read_user_input();
		if (!line)
			break ;
		handle_history(line);
		continue_loop = process_line(line, my_envp, last_status);
		free(line);
		if (!continue_loop)
			break ;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*my_envp;
	int		last_status;

	(void)argv;
	if (argc != 1)
		return (printf("Run only whit ./minishell\n"), 1);
	setup_interactive_signals();
	if (init_environment(envp, &my_envp))
		return (1);
	last_status = 0;
	main_loop(&my_envp, &last_status);
	cleanup_and_exit(&my_envp, last_status);
	return (last_status);
}
