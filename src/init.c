/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 15:38:37 by alvanaut          #+#    #+#             */
/*   Updated: 2025/07/31 17:35:45 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


t_token *init_token(char *value, t_token_type type)
{
    t_token *token;
    token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    if (!value)
        return (NULL);
    token->value = ft_strdup(value);
    token->type = type;
    token->next = NULL;
    token->prec = NULL;
    return (token);
}

void    free_token(t_token *token)
{
    if (!token)
        return ;
    free(token->value);
    free(token);
}

