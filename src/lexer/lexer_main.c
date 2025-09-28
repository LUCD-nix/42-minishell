/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 16:55:59 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:45:19 by alvanaut         ###   ########.fr       */
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

static t_lexeme	get_lexeme_by_type(char *line, int *i, int separator_type)
{
	if (separator_type == 1)
		return (handle_separator(&line[*i], i));
	else if (separator_type == 2)
		return (handle_quote(&line[*i], i));
	else
		return (handle_word(&line[*i], i));
}

static int	should_add_lexeme(t_lexeme lex, t_lexeme **lexeme, int *count)
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

static int	process_current_position(char *line, int *i, t_lexeme **lexeme,
		int *count)
{
	t_lexeme	lex;
	int			separator_type;

	skip_spaces(line, i);
	if (!line[*i])
		return (1);
	separator_type = is_separator(&line[*i]);
	lex = get_lexeme_by_type(line, i, separator_type);
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
