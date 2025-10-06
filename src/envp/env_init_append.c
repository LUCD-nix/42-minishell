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
#include "../../minishell.h"

int	env_concat(char *str, t_list *env)
{
	char	*cursor;
	char	*value;

	cursor = ft_strchr(str, '=');
	*(cursor - 1) = '\0';
	value = env_get(env, str);
	if (value == NULL)
		return (EXIT_FAILURE);
	value = ft_strjoin(value, cursor + 1);
	if (!value)
		return (EXIT_FAILURE);
	return (env_set(env, str, value));
}

t_env	env_from_str(char *str)
{
	int		key_len;
	char	*cursor;
	char	*key;
	char	*value;

	cursor = ft_strchr(str, '=');
	if (cursor == NULL)
	{
		key = ft_strdup(str);
		value = ft_calloc(1, 1);
		return ((t_env){.key = key, .value = value});
	}
	key_len = cursor - str;
	key = ft_calloc(key_len + 1, sizeof(char));
	ft_strlcpy(key, str, key_len + 1);
	cursor++;
	value = ft_calloc(ft_strlen(cursor) + 1, sizeof(char));
	ft_strlcpy(value, cursor, ft_strlen(cursor) + 1);
	return ((t_env){.key = key, .value = value});
}

t_list	*env_lst_add(t_list **lst, char *str)
{
	t_env	*tmp;

	tmp = ft_calloc(1, sizeof(t_env));
	if (tmp == NULL)
		return (NULL);
	*tmp = env_from_str(str);
	ft_lstadd_back(lst, ft_lstnew(tmp));
	return (*lst);
}

t_list	*env_lst_from_str_arr(char **to_copy)
{
	t_list	*res;
	char	*temp;
	int		i;

	i = 0;
	res = NULL;
	if (to_copy == NULL)
	{
		temp = "PWD=";
		temp = ft_strjoin_free_second(temp, getcwd(NULL, 0));
		env_lst_add(&res, temp);
		free(temp);
	}
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
