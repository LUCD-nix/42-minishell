/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handler.c - FIXED VERSION                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 16:55:53 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 00:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	find_quote_end(char *line, char quote)
{
	int	index;

	index = 1;
	while (line[index] && line[index] != quote)
		index++;
	return (index);
}

t_quote_type	get_quote_type(char quote)
{
	if (quote == '\'')
		return (Q_SIMPLE);
	else if (quote == '"')
		return (Q_DOUBLE);
	return (Q_NONE);
}

t_lexeme	handle_quote(char *line, int *i)
{
	t_lexeme	lex;
	char		quote;
	int			index;

	lex = (t_lexeme){NULL, Q_NONE};
	if (!line)
		return (lex);
	quote = line[0];
	if (quote != '\'' && quote != '"')
		return (lex);
	index = find_quote_end(line, quote);
	if (!line[index])
	{
		printf("minishell: syntax error: unexpected end of file\n");
		*i += index;
		return (lex);
	}
	lex.value = ft_substr(line, 1, index - 1);
	lex.quote = get_quote_type(quote);
	*i += index + 1;
	return (lex);
}

int	skip_quoted_section(char *line, int index)
{
	char	quote;
	int		start;

	quote = line[index];
	start = index;
	index++;
	while (line[index] && line[index] != quote)
		index++;
	if (line[index] == quote)
		index++;
	return (index - start);
}

t_lexeme	handle_word(char *line, int *i)
{
	int			index;
	t_lexeme	lex;

	lex = (t_lexeme){NULL, Q_NONE};
	index = 0;
	while (line[index] && !is_separator(&line[index]) && line[index] != ' ')
	{
		if (line[index] == '\'' || line[index] == '"')
		{
			index += skip_quoted_section(&line[index], 0);
		}
		else
		{
			if (line[index] == '\\' && line[index + 1])
				index++;
			index++;
		}
	}
	*i += index;
	lex.value = ft_substr(line, 0, index);
	return (lex);
}
