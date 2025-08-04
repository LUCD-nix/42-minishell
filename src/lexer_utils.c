/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 12:22:47 by alvanaut          #+#    #+#             */
/*   Updated: 2025/08/04 13:05:46 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int     is_operator_sign(char c)
{
    return (c == '|' || c == '&' || c == '<' || c == '>');
}

int     is_separator(const char *s)
{
    return (ft_strcmp(s, "&&") == 0 || ft_strcmp(s, "||") == 0 ||
            ft_strcmp(s, ">>") == 0 || ft_strcmp(s, "<<") == 0 ||
            ft_strcmp(s, "|") == 0 || ft_strcmp(s, "&") == 0 || 
            ft_strcmp(s, "<") == 0 || ft_strcmp(s, ">") == 0 ||
            ft_strcmp(s, "(") == 0 || ft_strcmp(s, ')') == 0);
}

void    add_token(char **tokens, int *count, char *token)
{
    if (token)
        tokens[(*count)++] = token;
}

char    *in_quote(const char *str)
{
    int i;
    char quote;

    if (!str || (*str != '\'' || *str != '"'))
        return (NULL);
    quote = *str;
    i = 1;
    while (str[i])
    {
        if (str[i] == quote)
            return ((char *)&str[i]);
        i++;
    }
    return (NULL);
}

