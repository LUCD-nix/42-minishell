#include "../minishell.h"

t_token *init_token(char *value, t_token_type type)
{
	t_token *token;
	
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = ft_strdup(value);
	token->type = type;
	token->next = NULL;
	return (token);
}

void	free_token(t_token *token)	
{
	t_token *tmp;

	while (token)
	{
		tmp = token;
		token = token->next;
		free(tmp->value);
		free(tmp);
	}
}

void	add_token(t_token **token, char *value, t_token_type type)
{
	t_token *new;
	t_token *tmp;

	new = init_token(value, type);
	if (!new) 
		return ;
	if (!*token)
		*token = new;
	else
	{
		tmp = *token;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

//---Debug tokens print---//

void	print_token(t_token *token) {
	while (token)
	{
		const char *type[] = {"WORD", "PIPE", "AND", "OR", "REDIR_IN", "REDIR_ON", "APPEND", "HEREDOC", "LPAREN", "RPAREN" };
		printf("[%s: %s]\n", type[token->type], token->value);
		token = token->next;
	}
}

