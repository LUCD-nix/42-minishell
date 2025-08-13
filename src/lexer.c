/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 17:48:33 by alvanaut          #+#    #+#             */
/*   Updated: 2025/08/06 17:48:36 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <readline/history.h>

// Analyser l input de readline
// Regarder le type de l input (separator, word) et lui donner le bon type
// Pas de split mon seulement des substr, plus simple pour le lexer

char *read_line(void)
{
	char *line;
	line = readline("Minishell $");
	if (!line || !ft_strcmp(line, "exit"))
		return (NULL);
	return (line);
}

void	free_split(char **tab)
{
	int i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}


char *handle_quote(const char *str)
{
	int i;
	char quote;

	quote = str[0];
	i = 1;
	while (str[i])
	{
		if (str[i] == quote)
			return (ft_substr(str, 0, i + 1));
		i++;
	}
	return (printf("Error: syntax error.\n"), NULL);
}

char	*is_operator(const char *s, int len)
{
	if (!s)
		return (NULL);
	if (ft_strncmp(s, "|", len) == 0 || ft_strncmp(s, "||", len) == 0 || 
		ft_strncmp(s, "&", len) == 0 || ft_strncmp(s, "&&", len) == 0 ||
		ft_strncmp(s, ">", len) == 0 || ft_strncmp(s, ">>", len) == 0 ||
		ft_strncmp(s, "<", len) == 0 || ft_strncmp(s, "<<", len) == 0 ||
		ft_strncmp(s, "(", len) == 0 || ft_strncmp(s, ")", len) == 0 ||
		ft_strncmp(s, ";",len) == 0)
			return (s += len, ft_substr(s, 0, len));
	return (NULL);
}

int		sep_len(const char *s)
{
	int i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
	{
		if ((s[i] == '|' && s[i + 1] == '|') || (s[i] == '&' || s[i + 1] == '&') ||
			(s[i] == '>' || s[i + 1] == '>') || (s[i] == '<' || s[i + 1] == '<'))
			return (2);
		else if ((s[i] == '|' && s[i + 1] != '|') || (s[i] == '&' || s[i + 1] != '&') ||
			(s[i] == '>' || s[i + 1] != '>') || (s[i] == '<' || s[i + 1] != '<') || 
			s[i] == ';')
			return (1);
		i++;
	}
	return (0);
}
