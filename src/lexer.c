#include "../minishell.h"

char *read_line(void)
{
	char *line;

	line = readline("$Minishell ");
	if (!line)
		return (NULL);
	if (ft_strcmp(line, "exit") == 0)
		return (rl_free_line_state(), rl_clear_history(), free(line), NULL);
	add_history(line);
	return (line);
}

static t_lexeme handle_separator(char *line, int *i)
{
	t_lexeme lex;

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

static t_lexeme handle_quote(char *line, int *i)
{
	t_lexeme lex;
	char quote;
	int index;

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
		return (printf("Syntax error: Unclosed quote\n"), lex);
	lex.value = ft_substr(line, 1, index - 1);
	if (quote == '\'')
		lex.quote = Q_SIMPLE;
	else
		lex.quote = Q_DOUBLE;
	*i += index + 1;
	return (lex);
}

static int		is_separator(char *line)
{
	if (!line)
		return (0);
	if (ft_strncmp(line, "||", 2) == 0 || ft_strncmp(line, "&&", 2) == 0 || ft_strncmp(line, ">>", 2) == 0 ||
		ft_strncmp(line, "<<", 2) == 0 ) 
		return (1);
	if(ft_strncmp(line, ")", 1) == 0 || ft_strncmp(line, "(", 1) == 0 || ft_strncmp(line, "|", 1) == 0 ||
		ft_strncmp(line, ">", 1) == 0 || ft_strncmp(line, "<", 1) == 0 || ft_strncmp(line, ";", 1) == 0 || 
		ft_strncmp(line, "&", 1) == 0)
		return (1);
	if (line[0] == '\'' || line[0] == '"')
		return (2);
	return (0);
}

static t_lexeme handle_word(char *line, int *i)
{
	int index;
	t_lexeme lex;

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

void	skip_spaces(char *line, int *i)
{
	while (line[*i] == '\t' || line[*i] == ' ')
		(*i)++;
}

t_lexeme *add_lexeme(t_lexeme *lexeme, t_lexeme lex, int count)
{
	t_lexeme *new_lexeme;
	int i;

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
	int i;

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

t_lexeme *lexer(char *line)
{
	t_lexeme lex;
	t_lexeme *lexeme = NULL;
	int i;
	int count;

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

// int main(void)
// {
//     char *line;
//     t_lexeme *lexemes;
//     int i;
//
//     while (1)
//     {
//         line = read_line(); // suppose que read_line renvoie une ligne allouée
//         if (!line)
//             break ; // Ctrl+D ou erreur
//
//         lexemes = lexer(line);
//         free(line);
//
//         if (!lexemes)
//             continue ;
//
//         i = 0;
//         while (lexemes[i].value)
//         {
//             printf("Lexeme[%d]: \"%s\" Quote: %d\n", i, lexemes[i].value, lexemes[i].quote);
//             free(lexemes[i].value);
//             i++;
//         }
//         free(lexemes);
//     }
//
//     return 0;
// }







