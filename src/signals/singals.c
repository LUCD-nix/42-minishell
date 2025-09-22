#include "../../minishell.h"

void	handle_sigint_interactive(int sig)
{
	g_signal_received = sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
}

void	handle_sigint_child(int sig)
{
	g_signal_received = sig;
	write(STDOUT_FILENO, "\n", 1);
}

void	handle_sigquit_child(int sig)
{
	g_signal_received = sig;
	write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
}

void	setup_interactive_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// SIGINT (Ctrl+C) en mode interactif
	sa_int.sa_handler = handle_sigint_interactive;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);

	// SIGQUIT (Ctrl+\) ignoré en mode interactif  
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

void	setup_child_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// SIGINT pour les processus enfants
	sa_int.sa_handler = handle_sigint_child;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);

	// SIGQUIT pour les processus enfants
	sa_quit.sa_handler = handle_sigquit_child;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

void	setup_heredoc_signals(void)
{
	struct sigaction	sa_int;

	// SIGINT pour heredoc (doit interrompre)
	sa_int.sa_handler = SIG_DFL;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	
	// SIGQUIT ignoré
	signal(SIGQUIT, SIG_IGN);
}

int	ignore_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	if (sigaction(SIGINT, &sa, NULL) == -1)
		return (-1);
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		return (-1);
	return (0);
}

int	check_signal_status(void)
{
	if (g_signal_received == SIGINT)
	{
		g_signal_received = 0;
		return (130); // Code de sortie bash pour SIGINT
	}
	if (g_signal_received == SIGQUIT)
	{
		g_signal_received = 0;
		return (131); // Code de sortie bash pour SIGQUIT
	}
	return (0);
}
