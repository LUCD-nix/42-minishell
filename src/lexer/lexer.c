#include "../../minishell.h"

char	*read_line(void)
{
	char	*line;

	line = readline("$Minishell ");
	if (!line)
		return (NULL);
	if (ft_strcmp(line, "exit") == 0)
		return (rl_free_line_state(), rl_clear_history(), free(line), NULL);
	add_history(line);
	return (line);
}

static t_lexeme	handle_separator(char *line, int *i)
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

static t_lexeme	handle_quote(char *line, int *i)
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
	{
		printf("minishell: syntax error: unclosed quote\n");
		*i += index;
		return (lex);
	}
	if (quote == '"')
	{
		lex.value = ft_substr(line, 0, index + 1);
		lex.quote = Q_DOUBLE;
	}
	else
	{
		lex.value = ft_substr(line, 1, index - 1);
		lex.quote = Q_SIMPLE;
	}
	*i += index + 1;
	return (lex);
}

static int	is_separator(char *line)
{
	if (!line)
		return (0);
	if (ft_strncmp(line, "||", 2) == 0 || ft_strncmp(line, "&&", 2) == 0
		|| ft_strncmp(line, ">>", 2) == 0 || ft_strncmp(line, "<<", 2) == 0)
		return (1);
	if (ft_strncmp(line, ")", 1) == 0 || ft_strncmp(line, "(", 1) == 0
		|| ft_strncmp(line, "|", 1) == 0 || ft_strncmp(line, ">", 1) == 0
		|| ft_strncmp(line, "<", 1) == 0 || ft_strncmp(line, ";", 1) == 0
		|| ft_strncmp(line, "&", 1) == 0)
		return (1);
	if (line[0] == '\'' || line[0] == '"')
		return (2);
	return (0);
}

static t_lexeme	handle_word(char *line, int *i)
{
	int			index;
	t_lexeme	lex;
	int			in_quotes;
	char		quote_char;

	lex = (t_lexeme){NULL, Q_NONE};
	index = 0;
	in_quotes = 0;
	quote_char = 0;
	
	while (line[index])
	{
		// Si on rencontre une quote et qu'on n'est pas déjà dans des quotes
		if (!in_quotes && (line[index] == '\'' || line[index] == '"'))
		{
			in_quotes = 1;
			quote_char = line[index];
			index++;
		}
		// Si on est dans des quotes et qu'on trouve la quote fermante
		else if (in_quotes && line[index] == quote_char)
		{
			in_quotes = 0;
			quote_char = 0;
			index++;
		}
		// Si on est dans des quotes, continuer peu importe le caractère
		else if (in_quotes)
		{
			index++;
		}
		// Si on n'est pas dans des quotes, vérifier les séparateurs et espaces
		else if (is_separator(&line[index]) || line[index] == ' ')
		{
			break;
		}
		// Gestion de l'échappement
		else if (line[index] == '\\' && line[index + 1])
		{
			index += 2;
		}
		else
		{
			index++;
		}
	}
	
	*i += index;
	lex.value = ft_substr(line, 0, index);
	return (lex);
}

static void	skip_spaces(char *line, int *i)
{
	while (line[*i] == '\t' || line[*i] == ' ')
		(*i)++;
}

static t_lexeme	*add_lexeme(t_lexeme *lexeme, t_lexeme lex, int count)
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

void	free_lexemes(t_lexeme *lex)
{
	int	i;

	if (!lex)
		return ;
	i = 0;
	while (lex[i].value)
	{
		free(lex[i].value);
		i++;
	}
	free(lex);
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

