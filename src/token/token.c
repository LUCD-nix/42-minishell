#include "../../minishell.h"

t_token	*init_token(const char *line, t_token_type type, t_quote_type quote)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	if (line)
	{
		new->value = ft_strdup(line);
		if (!new->value)
			return (free(new), NULL);
	}
	else
		new->value = NULL;
	new->type = type;
	new->quote = quote;
	new->next = NULL;
	return (new);
}

void	free_token(t_token *token)
{
	t_token	*tmp;

	while (token)
	{
		tmp = token;
		token = token->next;
		if (tmp->value)
			free(tmp->value);
		free(tmp);
	}
}

t_token_type	get_token_type(char *token)
{
	if (!token)
		return (T_EOF);
	if (ft_strncmp(token, "||", 2) == 0)
		return (T_OR);
	if (ft_strncmp(token, "&&", 2) == 0)
		return (T_AND);
	if (ft_strncmp(token, ">>", 2) == 0)
		return (T_APPEND);
	if (ft_strncmp(token, "<<", 2) == 0)
		return (T_HEREDOC);
	if (ft_strncmp(token, "|", 1) == 0)
		return (T_PIPE);
	if (ft_strncmp(token, "(", 1) == 0)
		return (T_LPAREN);
	if (ft_strncmp(token, ")", 1) == 0)
		return (T_RPAREN);
	if (ft_strncmp(token, ">", 1) == 0)
		return (T_REDIR_OUT);
	if (ft_strncmp(token, "<", 1) == 0)
		return (T_REDIR_IN);
	return (T_WORD);
}

t_token	*lexer_to_token(t_lexeme *lex)
{
	t_token	*head;
	t_token	*current;
	t_token	*new_node;
	int		i;

	if (!lex)
		return (NULL);
	head = NULL;
	current = NULL;
	i = 0;
	while (lex[i].value)
	{
		new_node = init_token(lex[i].value, get_token_type(lex[i].value), lex[i].quote);
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
