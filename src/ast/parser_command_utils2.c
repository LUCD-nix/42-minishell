/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_utils2.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:23:18 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:34:37 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	collect_single_arg(t_parser *parser, char ***args, int *capacity,
		int count)
{
	if (!handle_capacity_resize(args, capacity, count))
		return (0);
	if (!add_current_arg(args, count, parser))
		return (0);
	advance(parser);
	return (1);
}
