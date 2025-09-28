/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 16:55:59 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/26 16:56:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_lexeme	*add_lexeme(t_lexeme *lexeme, t_lexeme lex, int count)
{
	t_lexeme	*new_lexeme;
	int			i;

	i = 0;
	new_lexeme = malloc(sizeof(t_lexeme) * (count + 2));
	if (!new_lexeme)
		return (NULL);
	while (i < count)
	{
		new_lexeme[i] = lexeme[i];
		i++;
	}
	new_lexeme[count] = lex;
	new_lexeme[count + 1] = (t_lexeme){NULL, Q_NONE};
	free(lexeme);
	return (new_lexeme);
}

t_lexeme	*lexer(char *line)
{
	t_lexeme	lex;
	t_lexeme	*lexeme;
	int			i;
	int			count;

	lexeme = NULL;
	i = 0;
	count = 0;
	while (line[i])
	{
		skip_spaces(line, &i);
		if (!line[i])
			break ;
		if (is_separator(&line[i]) == 1)
			lex = handle_separator(&line[i], &i);
		else if (is_separator(&line[i]) == 2)
			lex = handle_quote(&line[i], &i);
		else
			lex = handle_word(&line[i], &i);
		if (lex.value)
		{
			lexeme = add_lexeme(lexeme, lex, count);
			count++;
		}
	}
	return (lexeme);
}
