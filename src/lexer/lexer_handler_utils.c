/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handler_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:46:27 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:46:47 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_lexeme	create_separator_lexeme(char *line, int start, int len, int *i)
{
	t_lexeme	lex;

	lex.value = ft_substr(line, start, len);
	lex.quote = Q_NONE;
	*i += len;
	return (lex);
}

t_lexeme	handle_double_separators(char *line, int *i)
{
	t_lexeme	empty_lex;

	empty_lex = (t_lexeme){NULL, Q_NONE};
	if (ft_strncmp(line, "||", 2) == 0)
		return (create_separator_lexeme(line, 0, 2, i));
	if (ft_strncmp(line, "&&", 2) == 0)
		return (create_separator_lexeme(line, 0, 2, i));
	if (ft_strncmp(line, ">>", 2) == 0)
		return (create_separator_lexeme(line, 0, 2, i));
	if (ft_strncmp(line, "<<", 2) == 0)
		return (create_separator_lexeme(line, 0, 2, i));
	return (empty_lex);
}

t_lexeme	handle_single_separators(char *line, int *i)
{
	t_lexeme	empty_lex;

	empty_lex = (t_lexeme){NULL, Q_NONE};
	if (ft_strncmp(line, "|", 1) == 0)
		return (create_separator_lexeme(line, 0, 1, i));
	if (ft_strncmp(line, "&", 1) == 0)
		return (create_separator_lexeme(line, 0, 1, i));
	if (ft_strncmp(line, ">", 1) == 0)
		return (create_separator_lexeme(line, 0, 1, i));
	if (ft_strncmp(line, "<", 1) == 0)
		return (create_separator_lexeme(line, 0, 1, i));
	if (ft_strncmp(line, "(", 1) == 0)
		return (create_separator_lexeme(line, 0, 1, i));
	if (ft_strncmp(line, ")", 1) == 0)
		return (create_separator_lexeme(line, 0, 1, i));
	if (ft_strncmp(line, ";", 1) == 0)
		return (create_separator_lexeme(line, 0, 1, i));
	return (empty_lex);
}

t_lexeme	handle_separator(char *line, int *i)
{
	t_lexeme	lex;

	if (!line)
		return ((t_lexeme){NULL, Q_NONE});
	lex = handle_double_separators(line, i);
	if (lex.value)
		return (lex);
	return (handle_single_separators(line, i));
}

t_lexeme	get_lexeme_by_type(char *line, int *i, int lexeme_type)
{
	if (lexeme_type == 1)
		return (handle_separator(&line[*i], i));
	else if (lexeme_type == 2)
		return (handle_quote(&line[*i], i));
	else
		return (handle_word(&line[*i], i));
}
