/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:55:33 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 18:30:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	handle_sigint_interactive(int sig)
{
	g_signal_received = sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sigint_child(int sig)
{
	g_signal_received = sig;
}

void	handle_sigquit_child(int sig)
{
	g_signal_received = sig;
}

void	handle_sigint_heredoc(int sig)
{
	char	c;

	g_signal_received = sig;
	write(STDOUT_FILENO, "\n", 1);
	c = '\0';
	ioctl(STDIN_FILENO, TIOCSTI, &c);
}
