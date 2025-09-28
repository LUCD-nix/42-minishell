/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 16:56:04 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/26 16:56:05 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

int	is_separator(char *line)
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

void	skip_spaces(char *line, int *i)
{
	while (line[*i] == '\t' || line[*i] == ' ')
		(*i)++;
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
