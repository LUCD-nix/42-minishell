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

// Fonction pour valider un nom de variable selon POSIX
static int	is_valid_identifier(char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	// Premier caractère doit être une lettre ou underscore
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	// Autres caractères doivent être alphanumériques ou underscore
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static char	*remove_quotes(char *str)
{
	int		len;
	char	*result;

	if (!str)
		return (NULL);
	
	len = ft_strlen(str);
	
	// Si la chaîne commence et finit par des guillemets doubles
	if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
	{
		result = ft_substr(str, 1, len - 2);
		return (result);
	}
	// Si la chaîne commence et finit par des guillemets simples
	else if (len >= 2 && str[0] == '\'' && str[len - 1] == '\'')
	{
		result = ft_substr(str, 1, len - 2);
		return (result);
	}
	// Sinon, retourner une copie de la chaîne originale
	return (ft_strdup(str));
}

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
		// Validation du nom de variable
		if (!is_valid_identifier(node->command->args[i]))
		{
			ft_printf("minishell: unset: `%s': not a valid identifier\n", 
					node->command->args[i]);
			continue;
		}
		key = key_from_args(node, i);
		if (key == NULL)
			return (EXIT_FAILURE);
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

static char	*key_from_args_cleaned(char *arg)
{
	int		j;
	char	*key;

	j = 0;
	while (arg[j] && arg[j] != '=')
		j++;
	key = ft_calloc(j + 1, 1);
	if (!key)
		return (NULL);
	ft_strlcpy(key, arg, j + 1);
	return (key);
}

int	builtin_export(int argc, t_ast *node)
{
	t_list	**env_lst;
	char	*key;
	char	*cleaned_arg;
	int		i;
	int		has_equals;

	if (argc == 1)
		return (export_no_args(node));
	
	env_lst = node->env;
	i = 0;
	while (++i < argc)
	{
		// NOUVEAU: Nettoyer les guillemets de l'argument
		cleaned_arg = remove_quotes(node->command->args[i]);
		if (!cleaned_arg)
			return (EXIT_FAILURE);
		
		has_equals = (ft_strchr(cleaned_arg, '=') != NULL);
		
		if (!is_valid_identifier(cleaned_arg))
		{
			ft_printf("minishell: export: `%s': not a valid identifier\n", 
					cleaned_arg);
			free(cleaned_arg);
			continue;
		}
		
		key = key_from_args_cleaned(cleaned_arg);
		if (!key)
		{
			free(cleaned_arg);
			return (EXIT_FAILURE);
		}
		
		if (!has_equals)
		{
			if (env_get(*env_lst, key) == NULL)
			{
				char *export_var = ft_strjoin(key, "");
				if (export_var)
				{
					env_lst_add(env_lst, export_var);
					free(export_var);
				}
			}
		}
		else
		{
			env_delete_key(env_lst, key);
			env_lst_add(env_lst, cleaned_arg);
		}
		free(key);
		free(cleaned_arg);
	}
	return (EXIT_SUCCESS);
}
