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
		if (ft_printf("export %s=\"%s\"\n", temp.key, temp.value) < 0)
			return (EXIT_FAILURE);
		cursor = cursor->next;
	}
	return (EXIT_SUCCESS);
}

static int	is_valid_identifier(char *str)
{
	int	i;

	if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
		return (0);
	i = 1;
	while (str[i] && str[i] != '=' && str[i] != '+')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
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
		if (!is_valid_identifier(node->command->args[i]))
		{
			ft_printf("minishell: export: `%s': not a valid identifier\n",
				node->command->args[i]);
			return (EXIT_FAILURE);
		}
		key = key_from_args(node, i);
		if (!key)
			return (EXIT_FAILURE);
		if (key[ft_strlen(key) - 1] == '+')
			return (free(key), env_concat(node->command->args[i], *env_lst));
		env_delete_key(env_lst, key);
		env_lst_add(env_lst, node->command->args[i]);
		free(key);
	}
	return (EXIT_SUCCESS);
}
