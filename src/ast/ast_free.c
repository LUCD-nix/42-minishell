/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_free.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:40 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:30:40 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	free_redir(t_redir *redir)
{
	t_redir	*tmp;

	while (redir)
	{
		tmp = redir;
		redir = redir->next;
		if (tmp->filename)
			free(tmp->filename);
		free(tmp);
	}
}

void	free_cmd(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	if (cmd->path)
		free(cmd->path);
	free(cmd);
}

void	free_ast(t_ast *ast)
{
	if (ast == NULL)
		return ;
	free_cmd(ast->command);
	ast->command = NULL;
	if (ast->filename)
		free(ast->filename);
	ast->filename = NULL;
	free_ast(ast->left);
	ast->left = NULL;
	free_ast(ast->right);
	ast->right = NULL;
	free(ast);
}

int	is_builtin(char *value)
{
	if (!value)
		return (0);
	if (ft_strcmp(value, "echo") == 0 || ft_strcmp(value, "cd") == 0
		|| ft_strcmp(value, "pwd") == 0 || ft_strcmp(value, "export") == 0
		|| ft_strcmp(value, "unset") == 0 || ft_strcmp(value, "env") == 0
		|| ft_strcmp(value, "exit") == 0)
		return (1);
	return (0);
}

t_command	*allocate_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argc = 0;
	cmd->args = NULL;
	cmd->path = NULL;
	return (cmd);
}
