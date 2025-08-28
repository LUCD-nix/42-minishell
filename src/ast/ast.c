#include "../../minishell.h"

t_ast	*init_ast_node(t_node_type type)
{
	t_ast	*ast;

	ast = malloc(sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = type;
	ast->command = NULL;
	ast->filename = NULL;
	ast->fd_in = -1;
	ast->fd_out = -1;
	ast->left = NULL;
	ast->right = NULL;
	return (ast);
}

t_ast	*init_cmd_node(t_command *cmd)
{
	t_ast	*ast;

	ast = init_ast_node(NODE_CMD);
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
	if (!ast)
		return ;
	free_cmd(ast->command);
	if (ast->filename)
		free(ast->filename);
	free_ast(ast->left);
	free_ast(ast->right);
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

