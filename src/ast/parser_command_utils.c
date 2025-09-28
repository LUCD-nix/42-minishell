/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 12:58:15 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:09:18 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	**resize_args_array(char **args, int *capacity)
{
	char	**new_args;
	int		i;

	*capacity *= 2;
	new_args = malloc(sizeof(char *) * (*capacity));
	if (!new_args)
		return (NULL);
	i = 0;
	while (args && args[i])
	{
		new_args[i] = args[i];
		i++;
	}
	free(args);
	return (new_args);
}

char	**init_args_array(int *capacity)
{
	char	**args;

	*capacity = 4;
	args = malloc(sizeof(char *) * (*capacity));
	return (args);
}

int	handle_capacity_resize(char ***args, int *capacity, int count)
{
	if (count >= *capacity - 1)
	{
		*args = resize_args_array(*args, capacity);
		if (!*args)
			return (0);
	}
	return (1);
}

int	add_current_arg(char ***args, int count, t_parser *parser)
{
	(*args)[count] = ft_strdup(parser->current->value);
	if (!(*args)[count])
	{
		ft_free_tab(*args);
		return (0);
	}
	return (1);
}

int	should_continue_collection(t_parser *parser)
{
	if (!parser->current)
		return (0);
	if (get_precedence(parser->current->type) > PREC_NONE)
		return (0);
	return (1);
}
