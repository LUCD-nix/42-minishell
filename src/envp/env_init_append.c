/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init_append.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 10:48:27 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/27 10:48:28 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*   Correction pour env_init_append.c                                       */
/* ************************************************************************** */

#include "../../minishell.h"

// Version corrigée qui vérifie la validité du format
t_env	env_from_str(char *str)
{
	int		key_len;
	char	*cursor;
	char	*key;
	char	*value;

	if (!str)
		return ((t_env){.key = NULL, .value = NULL});
		
	cursor = ft_strchr(str, '=');
	if (cursor == NULL)
	{
		// Pas de '=' - créer une variable sans valeur
		key = ft_strdup(str);
		value = ft_strdup(""); // Valeur vide mais la variable existe
		return ((t_env){.key = key, .value = value});
	}
	
	key_len = cursor - str;
	if (key_len <= 0)
	{
		// Format invalide (commence par '=')
		return ((t_env){.key = NULL, .value = NULL});
	}
	
	key = ft_calloc(key_len + 1, sizeof(char));
	if (!key)
		return ((t_env){.key = NULL, .value = NULL});
		
	ft_strlcpy(key, str, key_len + 1);
	cursor++; // Passer le '='
	
	value = ft_strdup(cursor);
	if (!value)
	{
		free(key);
		return ((t_env){.key = NULL, .value = NULL});
	}
	
	return ((t_env){.key = key, .value = value});
}

t_list	*env_lst_add(t_list **lst, char *str)
{
	t_env	*tmp;
	t_env	env_entry;

	env_entry = env_from_str(str);
	if (!env_entry.key) // Vérifier si la création a échoué
		return (*lst);
		
	tmp = ft_calloc(1, sizeof(t_env));
	if (tmp == NULL)
	{
		free(env_entry.key);
		free(env_entry.value);
		return (NULL);
	}
	*tmp = env_entry;
	ft_lstadd_back(lst, ft_lstnew(tmp));
	return (*lst);
}

// Les autres fonctions restent identiques...
t_list	*env_lst_from_str_arr(char **to_copy)
{
	t_list	*res;
	int		i;

	i = 0;
	res = NULL;
	while (to_copy[i] != NULL)
	{
		if (env_lst_add(&res, to_copy[i]) == NULL)
			return (NULL);
		i++;
	}
	return (res);
}

char	**env_lst_to_str_array(t_list *env)
{
	t_list	*cursor;
	char	**res;
	char	*temp;
	int		size;
	int		i;

	cursor = env;
	size = ft_lstsize(env);
	res = ft_calloc(size + 1, sizeof(*res));
	if (res == NULL)
		return (NULL);
	i = 0;
	while (i < size)
	{
		temp = ft_strjoin(((t_env *)env->content)->key, "=");
		if (temp == NULL)
			return (NULL);
		res[i] = ft_strjoin_free_first(temp, ((t_env *)env->content)->value);
		if (res[i] == NULL)
			return (NULL);
		env = env->next;
		i++;
	}
	return (res);
}
