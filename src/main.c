#include "../minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

static char	*remove_outer_quotes(char *str)
{
	int		len;
	char	*result;

	if (!str)
		return (NULL);
	
	len = ft_strlen(str);
	
	// Si la chaîne commence et finit par des guillemets
	if (len >= 2 && 
		((str[0] == '"' && str[len - 1] == '"') || 
		 (str[0] == '\'' && str[len - 1] == '\'')))
	{
		result = ft_substr(str, 1, len - 2);
		return (result);
	}
	return (ft_strdup(str));
}

static void	expand_tokens(t_token *tokens, t_list *envp, int last_status)
{
	t_token	*tmp;
	char	*expanded;
	char	*cleaned;

	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == T_WORD)
		{
			expanded = expand_variables(tmp->value, envp, last_status, tmp->quote);
			if (expanded)
			{
				// Si le token avait des guillemets, les enlever maintenant
				if (tmp->quote == Q_DOUBLE || tmp->quote == Q_SIMPLE)
				{
					cleaned = remove_outer_quotes(expanded);
					free(expanded);
					expanded = cleaned;
				}
				
				free(tmp->value);
				tmp->value = expanded;
			}
		}
		tmp = tmp->next;
	}
}

static int	process_line_new(char *line, t_list **envp, int *last_status)
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
	ast = parse_new(tokens, envp);
	free_token(tokens);
	if (!ast)
		return (*last_status = 1, 1);  // FIX: erreur de parsing = 1
	*last_status = traverse_node_new(ast);
	free_ast(ast);
	return (1);
}

static void	check_eof_signal(char *line, t_list **envp)
{
	if (!line)
	{
		ft_printf("exit\n");
		ft_lstclear(envp, &env_free);
		rl_clear_history();
		exit(0);
	}
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_list	*my_envp;
	int		last_status;

	(void)argc;
	(void)argv;
	
	setup_interactive_signals();
	my_envp = env_lst_from_str_arr(envp);
	if (!my_envp)
		return (perror("Error: Failed to copy environment\n"), 1);
	
	last_status = 0;	
	while (1)
	{
		// FIX: Ne pas vérifier les signaux ici, laisser readline les gérer
		line = readline("$Minishell :");
		
		check_eof_signal(line, &my_envp);
		
		if (ft_strlen(line) == 0)
		{
			free(line);
			continue;
		}
		
		add_history(line);
		
		if (ft_strcmp(line, "exit") == 0)
		{
			free(line);
			ft_printf("exit\n");
			break;
		}
		
		if (!process_line_new(line, &my_envp, &last_status))
			break;
		
		free(line);
	}
	
	ft_lstclear(&my_envp, &env_free);
	rl_clear_history();
	return (last_status);
}
