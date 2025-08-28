#include "../minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

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

static char	**copy_envp(char **envp)
{
	char	**new_envp;
	int		i;
	int		count;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			ft_free_tab(new_envp);
			return (NULL);
		}
		i++;
	}
	new_envp[i] = NULL;
	return (new_envp);
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

static void	print_ast(t_ast *ast, int depth)
{
	int	i;

	if (!ast)
		return ;
	i = 0;
	while (i < depth)
	{
		printf("  ");
		i++;
	}
	if (ast->type == NODE_CMD || ast->type == NODE_BUILTIN)
	{
		printf("CMD: %s", ast->command ? ast->command->path : "NULL");
		if (ast->command && ast->command->args)
		{
			i = 1;
			while (ast->command->args[i])
			{
				printf(" %s", ast->command->args[i]);
				i++;
			}
		}
		printf("\n");
	}
	else if (ast->type == NODE_PIPE)
		printf("PIPE\n");
	else if (ast->type == NODE_AND)
		printf("AND\n");
	else if (ast->type == NODE_OR)
		printf("OR\n");
	else if (ast->type == NODE_SUBSHELL)
		printf("SUBSHELL\n");
	else if (ast->type == NODE_REDIR_IN)
		printf("REDIR_IN: %s\n", ast->filename ? ast->filename : "NULL");
	else if (ast->type == NODE_REDIR_OUT)
		printf("REDIR_OUT: %s\n", ast->filename ? ast->filename : "NULL");
	else if (ast->type == NODE_APPEND)
		printf("APPEND: %s\n", ast->filename ? ast->filename : "NULL");
	else if (ast->type == NODE_HEREDOC)
		printf("HEREDOC: %s\n", ast->filename ? ast->filename : "NULL");
	print_ast(ast->left, depth + 1);
	print_ast(ast->right, depth + 1);
}

static int	process_line(char *line, t_list *envp, int *last_status)
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
	expand_tokens(tokens, envp, *last_status);
	printf("\n=== TOKENS ===\n");
	print_tokens(tokens);
	ast = parse(tokens);
	free_token(tokens);
	if (!ast)
	{
		printf("Parse error: Invalid syntax\n");
		*last_status = 2;
		return (1);
	}
	printf("\n=== AST ===\n");
	print_ast(ast, 0);
	printf("\n");
	free_ast(ast);
	*last_status = 0;
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
		return (printf("Error: Failed to copy environment\n"), 1);
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
		if (!process_line(line, my_envp, &last_status))
			break ;
		free(line);
	}
	ft_lstclear(&my_envp, &env_free);
	rl_clear_history();
	printf("Goodbye!\n");
	return (last_status);
}
