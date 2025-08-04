/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:20:59 by alvanaut          #+#    #+#             */
/*   Updated: 2025/08/04 16:59:57 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int handle_quotes(const char *line, char **tokens,int *count, int i)
{
    char *closing;
    int end;

    closing = in_quote(&line[i]);
    if (!closing)
        return (-1);
    end = closing - line;
    add_token(tokens, count, ft_substr(line, i + 1, end));
    return (end + 1);
}

static int handle_operator(const char *line, char **tokens,int *count,  int i)
{
    if (is_separator(&line[i]))
    {
        if ((line[i] == line[i + 1]) && (line[i] == '>' || line[i] == '<' ||
                    line[i] == '|' || line[i] == '&'))
        {
            add_token(tokens, count, ft_substr(line, i, i + 2));
            return (i += 2);
        }
        else
        {
            add_token(tokens, count, ft_substr(line, i, i + 1));
            return (i + 1);
        }
    }
    return (-1);
}

static int handle_word(const char *line, char **token, )
{
    int start;
    char quote;
    char *word;

    start = *i;
    while (line[*i] && !ft_ispace(line[*i]) && !is_operator_sign(line[*i]))
    {
        if (line[*i] == '\'' || line[*i] == '"')
        {
            quote = line[(*i)++];
            while (line[*i] && line[*i + 1])
}
