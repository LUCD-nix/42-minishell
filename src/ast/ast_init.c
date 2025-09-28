/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:36 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:30:33 by alvanaut         ###   ########.fr       */
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
	ast->heredoc_quoted = 0;
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

static int	count_args(char **args)
{
	int	count;

	count = 0;
	while (args[count])
		count++;
	return (count);
}

static char	**duplicate_args(char **args, int argc)
{
	char	**new_args;
	int		i;

	new_args = malloc(sizeof(char *) * (argc + 1));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i < argc)
	{
		new_args[i] = ft_strdup(args[i]);
		if (!new_args[i])
		{
			while (--i >= 0)
				free(new_args[i]);
			free(new_args);
			return (NULL);
		}
		i++;
	}
	new_args[i] = NULL;
	return (new_args);
}

t_command	*init_cmd(char **args)
{
	t_command	*cmd;

	if (!args || !args[0])
		return (NULL);
	cmd = allocate_command();
	if (!cmd)
		return (NULL);
	cmd->argc = count_args(args);
	cmd->args = duplicate_args(args, cmd->argc);
	if (!cmd->args)
		return (free(cmd), NULL);
	cmd->path = ft_strdup(args[0]);
	if (!cmd->path)
		return (free_cmd(cmd), NULL);
	return (cmd);
}
