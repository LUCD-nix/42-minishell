/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_commands.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 17:26:38 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 17:26:42 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	**collect_args(t_parser *parser, int *count)
{
	char	**args;
	int		capacity;
	char	*arg;

	*count = 0;
	capacity = 4;
	args = init_args_array(&capacity);
	if (!args)
		return (NULL);
	while (check(parser, T_WORD))
	{
		if (!handle_capacity_resize(&args, &capacity, *count))
			return (NULL);
		arg = concat_arg_tokens(parser);
		if (!arg)
			return (ft_free_tab(args), NULL);
		args[*count] = arg;
		(*count)++;
		if (!should_continue_collection(parser))
			break ;
	}
	args[*count] = NULL;
	return (args);
}

t_command	*create_command_from_args(char **args)
{
	t_command	*cmd;

	cmd = init_cmd(args);
	ft_free_tab(args);
	return (cmd);
}

t_ast	*create_command_node(t_command *cmd, t_list **env)
{
	t_ast	*node;

	if (!cmd)
		return (NULL);
	node = init_cmd_node(cmd, env);
	if (!node)
	{
		free_cmd(cmd);
		return (NULL);
	}
	return (node);
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
	cmd = create_command_from_args(args);
	if (!cmd)
	{
		error(parser, "Memory allocation failed");
		return (NULL);
	}
	node = create_command_node(cmd, env);
	if (!node)
	{
		error(parser, "Memory allocation failed");
		return (NULL);
	}
	return (node);
}
