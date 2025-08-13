#include "../minishell.h"

t_command *init_command(char *path, char **args)
{
	t_command *cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = args;
	cmd->path = path;
	return (cmd);
}

t_ast *init_ast(t_node_type type, t_command *cmd, int fd_in, int fd_out)
{
	t_ast *ast;
	
	ast = malloc(sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->type = type;
	ast->command = cmd;
	ast->fd_in = fd_in;
	ast->fd_out = fd_out;
	ast->left = NULL;
	ast->right = NULL;
	return (ast);
}

void	free_args(char **args)
{
	int i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}


void	free_cmd(t_command *cmd)
{
	if (!cmd)
		return ;

	if (cmd->args)
		free_args(cmd->args);
	if (cmd->path)
		free(cmd->path);
	free(cmd);
}

void free_ast(t_ast *ast)
{
	if (!ast)
		return ;

	if (ast->command)
		free_cmd(ast->command);
	if (ast->left)
		free_ast(ast->left);
	if (ast->right)
		free_ast(ast->right);
	free(ast);
}
