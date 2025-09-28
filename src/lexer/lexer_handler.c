/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 16:55:53 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/26 16:57:54 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_lexeme	handle_separator_simple(char *line, int *i)
{
	t_lexeme	lex;

	lex = (t_lexeme){NULL, Q_NONE};
	if (!line)
		return (lex);
	if (ft_strncmp(line, "||", 2) == 0)
		return (*i += 2, lex.value = ft_substr(line, 0, 2), lex);
	if (ft_strncmp(line, "|", 1) == 0)
		return (*i += 1, lex.value = ft_substr(line, 0, 1), lex);
	if (ft_strncmp(line, "&&", 2) == 0)
		return (*i += 2, lex.value = ft_substr(line, 0, 2), lex);
	if (ft_strncmp(line, "&", 1) == 0)
		return (*i += 1, lex.value = ft_substr(line, 0, 1), lex);
	if (ft_strncmp(line, ">>", 2) == 0)
		return (*i += 2, lex.value = ft_substr(line, 0, 2), lex);
	if (ft_strncmp(line, ">", 1) == 0)
		return (*i += 1, lex.value = ft_substr(line, 0, 1), lex);
	if (ft_strncmp(line, "<<", 2) == 0)
		return (*i += 2, lex.value = ft_substr(line, 0, 2), lex);
	if (ft_strncmp(line, "<", 1) == 0)
		return (*i += 1, lex.value = ft_substr(line, 0, 1), lex);
	if (ft_strncmp(line, "(", 1) == 0)
		return (*i += 1, lex.value = ft_substr(line, 0, 1), lex);
	if (ft_strncmp(line, ")", 1) == 0)
		return (*i += 1, lex.value = ft_substr(line, 0, 1), lex);
	if (ft_strncmp(line, ";", 1) == 0)
		return (*i += 1, lex.value = ft_substr(line, 0, 1), lex);
	return (lex);
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
	index = 1;
	if (quote != '\'' && quote != '"')
		return (lex);
	while (line[index] && line[index] != quote)
		index++;
	if (!line[index])
		return (printf("minishell: syntax error: unclosed quote\n"), *i += index, lex);
	lex.value = ft_substr(line, 1, index - 1);
	if (quote == '\'')
		lex.quote = Q_SIMPLE;
	else
		lex.quote = Q_DOUBLE;
	*i += index + 1;
	return (lex);
}

t_lexeme	handle_word(char *line, int *i)
{
	int			index;
	t_lexeme	lex;

	lex = (t_lexeme){NULL, Q_NONE};
	index = 0;
	while (line[index] && !is_separator(&line[index]) && line[index] != ' ')
	{
		if (line[index] == '\\' && line[index + 1])
			index++;
		index++;
	}
	return (*i += index, lex.value = ft_substr(line, 0, index), lex);
}
