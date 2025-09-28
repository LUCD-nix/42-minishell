/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_commands.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 12:58:08 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 12:58:09 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	**collect_args(t_parser *parser, int *count)
{
	char	**args;
	int		capacity;

	*count = 0;
	args = init_args_array(&capacity);
	if (!args)
		return (NULL);
	while (check(parser, T_WORD))
	{
		if (!handle_capacity_resize(&args, &capacity, *count))
			return (NULL);
		if (!add_current_arg(&args, *count, parser))
			return (NULL);
		(*count)++;
		advance(parser);
		if (!should_continue_collection(parser))
			break ;
	}
	args[*count] = NULL;
	return (args);
}

t_ast	*parse_command(t_parser *parser, t_list **env)
{
	char		**args;
	int			argc;
	t_command	*cmd;
	t_ast		*node;

	args = collect_args(parser, &argc);
	if (!args || argc == 0)
	{
		error(parser, "Expected command");
		return (NULL);
	}
	cmd = init_cmd(args);
	ft_free_tab(args);
	if (!cmd)
	{
		error(parser, "Memory allocation failed");
		return (NULL);
	}
	node = init_cmd_node(cmd, env);
	if (!node)
	{
		free_cmd(cmd);
		error(parser, "Memory allocation failed");
		return (NULL);
	}
	return (node);
}
