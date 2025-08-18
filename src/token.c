#include "../minishell.h"

t_token *init_token(const char *line, t_token_type type, t_quote_type quote)
{
	t_token *new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	if (line)
	{
		new->value = ft_strdup(line);
		if (!new->value)
			return (free_token(new), NULL);
	}
	new->type = type;
	new->quote = quote;
	new->next = NULL;
	return (new);
}

void	free_token(t_token *token)
{
	t_token *tmp;

	if (!token)
			return ;
	while (token)
	{
		tmp = token;
		token = token->next;
		if (tmp->value)
			free(tmp->value);
		free(tmp);
	}
}

// Find type

t_token_type token_type(char *token)
{
	if (!token)
		return (-1);
	if (ft_strncmp(token, "||", 2) == 0)
		return (T_OR);
	if (ft_strncmp(token, "&&", 2) == 0)
		return (T_AND);
	if (ft_strncmp(token, "<<", 2) == 0)
		return (T_APPEND);
	if (ft_strncmp(token, ">>", 2) == 0)
		return (T_HEREDOC);
	if (ft_strncmp(token, "|", 1) == 0)
		return (T_PIPE);
	if (ft_strncmp(token, "(", 1) == 0)
		return (T_LPAREN);
	if (ft_strncmp(token, ")", 1) == 0)
		return (T_RPAREN);
	if (ft_strncmp(token, ">", 1) == 0)
		return (T_REDIR_IN);
	if (ft_strncmp(token, "<", 1) == 0)
		return (T_REDIR_OUT);
	else
		return (T_WORD);
}

t_token *lexer_to_token(t_lexeme *lex)
{
	t_token *new_node;
	t_token *current;
	t_token *head;
	int i;

	if (!lex)
		return (NULL);
	head = NULL;
	current = NULL;
	i = 0;
	while (lex[i].value)
	{
		new_node = init_token(lex[i].value, token_type(lex[i].value), lex[i].quote);
		if (!new_node)
			return (free_token(head), NULL);
		if (!head)
		{
			head = new_node;
			current = new_node;
		}
		else
		{
			current->next = new_node;
			current = current->next;
		}
		i++;
	}
	return (head);
}

int main(void)
{
	char		*line;
	t_lexeme	*lex;
	t_token		*tokens;
	t_token		*tmp;

	while (1)
	{
		line = read_line(); // récupère la ligne entrée par l'utilisateur
		if (!line)
			break; // CTRL+D ou EOF

		// 1. Lexer : transforme line en tableau de lexèmes
		lex = lexer(line);
		free(line);
		if (!lex)
			continue;

		// 2. Convertir lexèmes en liste de tokens
		tokens = lexer_to_token(lex);
		free_lexemes(lex); // à toi d’implémenter pour libérer lex[i].value et le tableau

		if (!tokens)
			continue;

		// 3. Affichage des tokens (debug)
		printf("=== TOKENS ===\n");
		tmp = tokens;
		while (tmp)
		{
			printf("value = [%s], type = %d, quote = %d\n",
				tmp->value, tmp->type, tmp->quote);
			tmp = tmp->next;
		}

		// 4. Libération
		free_token(tokens);
	}
	return (0);
}
