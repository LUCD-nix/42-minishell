/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 16:55:59 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/02 16:30:00 by alvanaut         ###   ########.fr       */
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

int	get_lexeme_type(char *line, int i)
{
	int	sep_type;

	sep_type = is_separator(&line[i]);
	if (sep_type)
		return (1);
	return (0);
}

int	should_add_lexeme(t_lexeme lex, t_lexeme **lexeme, int *count)
{
	if (lex.value)
	{
		*lexeme = add_lexeme(*lexeme, lex, *count);
		if (!*lexeme)
			return (0);
		(*count)++;
	}
	return (1);
}

int	process_current_position(char *line, int *i, t_lexeme **lexeme,
		int *count)
{
	t_lexeme	lex;
	int			lexeme_type;

	skip_spaces(line, i);
	if (!line[*i])
		return (1);
	lexeme_type = get_lexeme_type(line, *i);
	lex = get_lexeme_by_type(line, i, lexeme_type);
	return (should_add_lexeme(lex, lexeme, count));
}

t_lexeme	*lexer(char *line)
{
	t_lexeme	*lexeme;
	int			i;
	int			count;

	lexeme = NULL;
	i = 0;
	count = 0;
	while (line[i])
	{
		if (!process_current_position(line, &i, &lexeme, &count))
			return (NULL);
		if (!line[i])
			break ;
	}
	return (lexeme);
}
