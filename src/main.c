#include "../minishell.h"

int g_signal_received = 0;

static void	handle_signal(int sig)
{
	g_signal_received = sig;
}

static void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

static void	handle_signals_interactive(void)
{
	if (g_signal_received == SIGINT)
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_signal_received = 0;
	}
}

static void	expand_tokens(t_token *tokens, t_list *envp, int last_status)
{
	t_token	*tmp;
	char	*expanded;

	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == T_WORD)
		{
			expanded = expand_variables(tmp->value, envp, last_status, tmp->quote);
			if (expanded)
			{
				free(tmp->value);
				tmp->value = expanded;
			}
		}
		tmp = tmp->next;
	}
}

static int	process_line(char *line, t_list **envp, int *last_status)
{
	t_lexeme	*lexemes;
	t_token		*tokens;
	t_ast		*ast;

	if (!line)
		return (0);
	if (ft_strlen(line) == 0)
		return (1);
	lexemes = lexer(line);
	if (!lexemes)
		return (1);
	tokens = lexer_to_token(lexemes);
	free_lexemes(lexemes);
	if (!tokens)
		return (1);
	expand_tokens(tokens, *envp, *last_status);
	ast = parse(tokens, envp);
	free_token(tokens);
	if (!ast)
		return (perror("Parse error: Invalid syntax\n"), *last_status = 2, 1);
	*last_status = traverse_node(ast);
	free_ast(ast);
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_list	*my_envp;
	int		last_status;

	(void)argc;
	(void)argv;
	setup_signals();
	my_envp = env_lst_from_str_arr(envp);
	if (!my_envp)
		return (perror("Error: Failed to copy environment\n"), 1);
	last_status = 0;
	printf("Minishell - Parsing Test Version\n");
	printf("Type 'exit' to quit\n\n");
	while (1)
	{
		handle_signals_interactive();
		line = readline("minishell$ ");
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (ft_strlen(line) > 0)
			add_history(line);
		if (ft_strcmp(line, "exit") == 0)
		{
			free(line);
			break ;
		}
		if (!process_line(line, &my_envp, &last_status))
			break ;
		free(line);
	}
	ft_lstclear(&my_envp, &env_free);
	rl_clear_history();
	printf("Goodbye!\n");
	return (last_status);
}
