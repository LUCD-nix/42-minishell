/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 11:05:37 by alvanaut          #+#    #+#             */
/*   Updated: 2025/08/04 11:25:51 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_ast	*init_ast(char **args, t_cmd_type type, t_redir *redir)
{
	t_ast	*root;

	root = malloc(sizeof(t_ast));
	if (!root)
		return (NULL);
	root->args = args;
	root->type = type;
	root->redir = redir;
	root->left = NULL;
	root->right = NULL;
	return (root);
}

t_redir	*init_redir(char *filename, t_redir_type type)
{
	t_redir	*new;

	new = malloc(sizeof(t_redir));
	if (!new)
		return (NULL);
	new->type = type;
	new->filename = ft_strdup(filename);
	new->next = NULL;
	return (new);
}

void	free_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_redir(t_redir *redir)
{
	t_redir	*tmp;

	while (redir)
	{
		tmp = redir;
		redir = redir->next;
		free(tmp->filename);
		free(tmp);
	}
}

void	free_ast(t_ast *root)
{
	if (!root)
		return ;
	if (root->args)
		free_args(root->args);
	if (root->redir)
		free_redir(root->redir);
	if (root->left)
		free_ast(root->left);
	if (root->right)
		free_ast(root->right);
	free(root);
}
