#include "../minishell.h"

volatile sig_atomic_t	g_signal_received = 0;


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
		return (*last_status = 2, 1);
	*last_status = traverse_node(ast);
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
	int		signal_status;

	(void)argc;
	(void)argv;
	
	// Configuration initiale des signaux
	setup_interactive_signals();
	
	my_envp = env_lst_from_str_arr(envp);
	if (!my_envp)
		return (perror("Error: Failed to copy environment\n"), 1);
	
	last_status = 0;	
	while (1)
	{
		// Vérifier si un signal a été reçu
		signal_status = check_signal_status();
		if (signal_status)
			last_status = signal_status;
		
		line = readline("$Minishell :");
		
		// Gestion Ctrl+D (EOF)
		check_eof_signal(line, &my_envp);
		
		// Gestion ligne vide
		if (ft_strlen(line) == 0)
		{
			free(line);
			continue;
		}
		
		// Ajouter à l'historique
		add_history(line);
		
		// Vérification commande exit
		if (ft_strcmp(line, "exit") == 0)
		{
			free(line);
			ft_printf("exit\n");
			break;
		}
		
		// Traitement de la ligne
		if (!process_line(line, &my_envp, &last_status))
			break;
		
		free(line);
	}
	
	// Nettoyage final
	ft_lstclear(&my_envp, &env_free);
	rl_clear_history();
	return (last_status);
}
