/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:48:44 by alvanaut          #+#    #+#             */
/*   Updated: 2025/08/06 17:48:45 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char *parse_quote(const char *s, int *i) {
	int start;
	char quote;

	start = *i;
	quote = s[*i];
	(*i)++;
	while (s[*i] && s[*i] != quote)
		(*i)++;
	if (!s[*i])
		printf("Syntax error: unclosed quote\n", NULL);
	(*i)++;
	return (ft_substr(s, start, *i - start));
}


char *parse_word(const char *s, int *i) {
    int start;

	start = *i;
    while (s[*i] && !ft_isspace(s[*i]) &&
           !is_operator_char(s[*i]) &&
           s[*i] != '"' && s[*i] != '\'') {
        (*i)++;
    }
    return ft_substr(s, start, *i - start);
}


char *parse_operator(const char *s, int *i) {
	int len;
	char *op;

	len = is_operator_len(&s[*i]);
	op = ft_substr(s, *i, len);
	*i += len;
	return (op);
}

t_token *tokenize(const char *line)
{
	int i;
	t_token *token = NULL;
	char *q;
	char *op;
	char *word;

	i = 0;
	while (line[i])
	{
		if (ft_isspace(line[i]))
		{
			i++;
			continue ;
		}
		if (line[i] == '\'' || line[i] == '"')
		{
			q = parse_quote(line, &i);
			if (!q)
				return (free_token(token), NULL);
			add_token(&token, q, T_QUOTE);
		}
		else if (is_operator_len(&line[i]))
		{
			op = parse_operator(line, &i);
			if (!op)
				return (free_token(token), NULL);
			add_token(&token, op, parse_token_type(op));
		}
		else
		{
			word = parse_word(line, &i);
			add_token(&token, word, T_WORD);
		}
	}
	return (token);
}

int main(void)
{
	char *line;
	t_token *tokens;

	while ((line = readline("Minishell $ ")))
	{
		if (!line || strcmp(line, "exit") == 0)
			break;
		add_history(line);
		tokens = tokenize(line);
		if (tokens)
		{
			print_token(tokens);
			free_token(tokens);
		}
		free(line);
	}
	return (0);
}
