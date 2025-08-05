/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:20:59 by alvanaut          #+#    #+#             */
/*   Updated: 2025/08/05 11:57:15 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_quotes(const char *line, char **tokens, int *count, int i)
{
	char	*closing;
	int		end;

	closing = in_quote(&line[i]);
	if (!closing)
		return (-1);
	end = closing - line;
	add_token(tokens, count, ft_substr(line, i + 1, end - (i + 1)));
	return (end + 1);
}

static int	handle_operator(const char *line, char **tokens, int *count, int i)
{
	if (is_separator(&line[i]))
	{
		if ((line[i] == line[i + 1]) && (line[i] == '>' || line[i] == '<'
				|| line[i] == '|' || line[i] == '&'))
		{
			add_token(tokens, count, ft_substr(line, i, 2));
			return (i + 2);
		}
		else
		{
			add_token(tokens, count, ft_substr(line, i, 1));
			return (i + 1);
		}
	}
	return (-1);
}

void	handle_word(const char *line, char **tokens, int *count, int *i)
{
	int		start;
	char	quote;
	char	*word;

	start = *i;
	while (line[*i] && !ft_isspace(line[*i]) && !is_operator_sign(line[*i]))
	{
		if (line[*i] == '\'' || line[*i] == '"')
		{
			quote = line[*i];
			(*i)++;
			while (line[*i] && line[*i] != quote)
				(*i)++;
			if (line[*i] == quote)
				(*i)++;
		}
		else
			(*i)++;
	}
	word = ft_substr(line, start, *i - start);
	if (word)
		add_token(tokens, count, word);
}

int	process_token(const char *line, char **tokens, int *count, int i)
{
	int	ret;

	while (ft_isspace(line[i]))
		i++;
	if (!line[i])
		return (-1);
	if ((line[i] == '\'' || line[i] == '"') && in_quote(&line[i]))
	{
		ret = handle_quotes(line, tokens, count, i);
		if (ret == -1)
			return (-1);
		i = ret;
	}
	else if (is_separator(&line[i]))
		i = handle_operator(line, tokens, count, i);
	else
		handle_word(line, tokens, count, &i);
	return (i);
}

char	**split_line_smart(const char *line)
{
	char	**tokens;
	int		count;
	int		i;
	int		next;

	tokens = ft_calloc(MAX_TOKENS, sizeof(char *));
	if (!tokens)
		return (NULL);
	i = 0;
	count = 0;
	while (line[i])
	{
		next = process_token(line, tokens, &count, i);
		if (next == -1)
			break ;
		i = next;
	}
	tokens[count] = NULL;
	return (tokens);
}

/*
void	print_tokens(char **tokens)
{
	int i = 0;
	while (tokens && tokens[i])
	{
		printf("Token[%d]: \"%s\"\n", i, tokens[i]);
		i++;
	}
}

int	main(void)
{
	char	*line;
	char	**tokens;

	while (1)
	{
		line = read_line();
		if (!line)
			break ;
		tokens = split_line_smart(line);
		print_tokens(tokens);
		free(line);
		// Free each token
		for (int i = 0; tokens && tokens[i]; i++)
			free(tokens[i]);
		free(tokens);
	}
	return (0);
}*/
