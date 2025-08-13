#include "../minishell.h"

int is_operator_char(char c)
{
	return (c == '|' || c == '&' || c == '>' || c == '<' || c == '(' || c == ')' || c == ';'); 
}

int is_operator_len(const char *s)
{
	if (!s)
		return (0);
	if ((s[0] == '|' && s[1] == '|') || (s[0] == '&' && s[1] == '&') || (s[0] == '>' && s[1] == '>')
	 	|| (s[0] == '<' && s[1] == '<'))
		return (2);
	if (is_operator_char(s[0]))
		return (1);
	return (0);
}

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t');
}

t_token_type parse_token_type(const char *op)
{
	if (ft_strcmp(op, "|") == 0)
		return (T_PIPE);
	if (ft_strcmp(op, "&&") == 0)
		return (T_AND);
	if (ft_strcmp(op, "||") == 0)
		return (T_OR);
	if (ft_strcmp(op, "<") == 0)
		return (T_REDIR_IN);
	if (ft_strcmp(op, ">") == 0)
		return (T_REDIR_OUT);
	if (ft_strcmp(op, ">>") == 0)
		return (T_APPEND);
	if (ft_strcmp(op, "<<") == 0)
		return (T_HEREDOC);
	if (ft_strcmp(op, "(") == 0)
		return (T_LPAREN);
	if (ft_strcmp(op, ")") == 0)
		return (T_RPAREN);
	return (T_WORD);
}
