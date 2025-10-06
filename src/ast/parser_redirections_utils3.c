/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections_utils3.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 18:02:47 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 18:02:49 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_filename	init_filename_result(t_parser *parser)
{
	t_filename	result;

	result.value = ft_strdup(parser->current->value);
	if (!result.value)
		return ((t_filename){NULL, 0});
	result.has_quotes = (parser->current->quote != Q_NONE);
	advance(parser);
	return (result);
}

int	contains_quotes(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

void	update_quotes_flag(t_parser *parser, t_filename *result)
{
	if (parser->current->quote != Q_NONE
		|| contains_quotes(parser->current->value))
		result->has_quotes = 1;
}
