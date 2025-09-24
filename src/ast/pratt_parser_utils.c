/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pratt_parser_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:15 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/24 14:38:16 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	error(t_parser *parser, char *message)
{
	parser->had_error = 1;
	printf("Parse error: %s\n", message);
	if (parser->current)
		printf("Near token: '%s'\n", parser->current->value);
}
