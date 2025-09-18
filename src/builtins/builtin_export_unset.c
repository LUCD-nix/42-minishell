/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_unset.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 10:31:32 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/27 10:31:36 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"
#include <stdlib.h>

static char	*key_from_args(t_ast *node, int i)
{
	int		j;
	char	*key;
	char	**args;

	j = 0;
	args = node->command->args;
	while (args[i][j] && args[i][j] != '=')
		j++;
	key = ft_calloc(j + 1, 1);
	if (!key)
		return (NULL);
	ft_strlcpy(key, node->command->args[i], j + 1);
	return (key);
}

int	builtin_unset(int argc, t_ast *node)
{
	t_list	**env_lst;
	char	*key;
	int		i;

	env_lst = node->env;
	i = 0;
	while (++i < argc)
	{
		key = key_from_args(node, i);
		if (key == NULL)
			return (EXIT_SUCCESS);
		env_delete_key(env_lst, key);
		free(key);
	}
	return (EXIT_SUCCESS);
}

static int	export_no_args(t_ast *node)
{
	t_list	*cursor;
	t_env	temp;

	cursor = *node->env;
	while (cursor != NULL)
	{
		temp = *(t_env *)cursor->content;
		if (ft_printf("declare -x %s=\"%s\"\n", temp.key, temp.value) < 0)
			return (EXIT_FAILURE);
		cursor = cursor->next;
	}
	return (EXIT_SUCCESS);
}

int	builtin_export(int argc, t_ast *node)
{
	t_list	**env_lst;
	char	*key;
	int		i;

	if (argc == 1)
		return (export_no_args(node));
	env_lst = node->env;
	i = 0;
	while (++i < argc)
	{
		key = key_from_args(node, i);
		if (!key)
			return (EXIT_FAILURE);
		env_delete_key(env_lst, key);
		env_lst_add(env_lst, node->command->args[i]);
		free(key);
	}
	return (EXIT_SUCCESS);
}
