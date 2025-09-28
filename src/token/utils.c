/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:58:21 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 14:07:31 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	ft_free_tab(char **tab)
{
	int	i;

	if (!tab)
		return ;
	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

void	*ft_realloc(void *ptr, size_t size)
{
	void	*new_ptr;

	if (!ptr)
		return (malloc(size));
	if (size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);
	ft_memcpy(new_ptr, ptr, size / 2);
	free(ptr);
	return (new_ptr);
}

t_token	*init_token(const char *line, t_token_type type, t_quote_type quote)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	if (line)
	{
		new->value = ft_strdup(line);
		if (!new->value)
			return (free(new), NULL);
	}
	else
		new->value = NULL;
	new->type = type;
	new->quote = quote;
	new->next = NULL;
	return (new);
}

void	free_token(t_token *token)
{
	t_token	*tmp;

	while (token)
	{
		tmp = token;
		token = token->next;
		if (tmp->value)
			free(tmp->value);
		free(tmp);
	}
}
