/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:36 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/24 14:38:37 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*init_ast_node(t_node_type type, t_list **env)
{
	t_ast	*ast;

	ast = malloc(sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = type;
	ast->command = NULL;
	ast->filename = NULL;
	ast->fd_in = STDIN_FILENO;
	ast->fd_out = STDOUT_FILENO;
	ast->env = env;
	ast->left = NULL;
	ast->right = NULL;
	return (ast);
}

t_ast	*init_cmd_node(t_command *cmd, t_list **env)
{
	t_ast	*ast;

	ast = init_ast_node(NODE_CMD, env);
	if (!ast)
		return (NULL);
	ast->command = cmd;
	if (cmd && is_builtin(cmd->args[0]))
		ast->type = NODE_BUILTIN;
	return (ast);
}

t_command	*init_cmd(char **args)
{
	t_command	*cmd;
	int			i;

	if (!args || !args[0])
		return (NULL);
	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argc = 0;
	while (args[cmd->argc])
		cmd->argc++;
	cmd->args = malloc(sizeof(char *) * (cmd->argc + 1));
	if (!cmd->args)
		return (free(cmd), NULL);
	i = 0;
	while (i < cmd->argc)
	{
		cmd->args[i] = ft_strdup(args[i]);
		if (!cmd->args[i])
			return (free_cmd(cmd), NULL);
		i++;
	}
	cmd->args[i] = NULL;
	cmd->path = ft_strdup(args[0]);
	if (!cmd->path)
		return (free_cmd(cmd), NULL);
	return (cmd);
}
