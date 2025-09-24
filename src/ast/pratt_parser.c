/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pratt_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:10 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/24 14:43:32 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_precedence	get_precedence(t_token_type type)
{
	if (type == T_OR)
		return (PREC_OR);
	if (type == T_AND)
		return (PREC_AND);
	if (type == T_PIPE)
		return (PREC_PIPE);
	if (type == T_REDIR_IN || type == T_REDIR_OUT || type == T_APPEND
		|| type == T_HEREDOC)
		return (PREC_REDIR);
	return (PREC_NONE);
}

void	advance(t_parser *parser)
{
	if (parser->current)
		parser->current = parser->current->next;
}

int	match(t_parser *parser, t_token_type type)
{
	if (check(parser, type))
	{
		advance(parser);
		return (1);
	}
	return (0);
}

int	check(t_parser *parser, t_token_type type)
{
	if (at_end(parser))
		return (type == T_EOF);
	return (parser->current->type == type);
}

int	at_end(t_parser *parser)
{
	return (parser->current == NULL);
}
