/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 16:08:36 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/18 16:08:39 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../minishell.h"

void	env_free(void *ptr)
{
	t_env	*env;

	env = (t_env *) ptr;
	free(env->key);
	free(env->value);
	free(env);
}

void env_delete_key(t_list **head, char *key)
{
	t_list	*temp;
	t_list	*prev;

	temp = *head;
    if (temp != NULL && !ft_strcmp(((t_env *)temp->content)->key, key))
	{
        *head = temp->next;
        ft_lstdelone(temp, &env_free);
        return ;
    }
    while (temp != NULL && ft_strcmp(((t_env *)temp->content)->key, key))
	{
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
        return;
    prev->next = temp->next;
	ft_lstdelone(temp, &env_free);
}

t_env	env_from_str(char *str)
{
	int		key_len;
	char	*cursor;
	char	*key;
	char	*value;

	// TODO : temp, ensure that export, e.g, TEST="$USER" is expanded correctly
	cursor = ft_strchr(str, '=');
	if (cursor == NULL)
	{
		key = ft_strdup(str);
		value = ft_calloc(1,1);
		return ((t_env){.key = key, .value = value});
	}
	key_len = cursor - str;
	key = ft_calloc(key_len + 1, sizeof(char));
	ft_strlcpy(key, str, key_len + 1);
	cursor++;
	value = ft_calloc(ft_strlen(cursor) + 1, sizeof(char));
	ft_strlcpy(value, cursor, ft_strlen(cursor) + 1);
	return ((t_env) {.key = key, .value = value});
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

char **env_lst_to_str_array(t_list *env)
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

char *env_get(t_list *env, char *key)
{
	size_t	key_len;
	t_env	*tmp;

	key_len = ft_strlen(key);
	while (env != NULL)
	{
		tmp = (t_env *)env->content;
		if (!ft_memcmp(tmp->key, key, key_len + 1))
		{
			return (tmp->value);
		}
		env = env->next;
	}
	return (NULL);
}

int	env_set(t_list *env, char *key, char *value)
{
	t_list	*cursor;
	t_env	*temp;
	size_t	key_len;

	cursor = env;
	if (value == NULL)
		return (-1);
	key_len = ft_strlen(key);
	while (cursor != NULL)
	{
		temp = (t_env *)cursor->content;
		if (ft_memcmp(key, temp->key, key_len + 1) == 0)
		{
			free(temp->value);
			temp->value = value;
			return (0);
		}
		cursor = cursor->next;
	}
	return (-1);
}

// int	main(void)
// {
// 	extern char	**environ;
// 	t_list		*env_list;
// 	char		**own_env;
//
// 	env_list = env_lst_from_str_arr(environ);
// 	env_lst_add(&env_list, "TEST=1");
// 	ft_lstiter(env_list, &print_env);
// 	own_env = env_lst_to_str_array(env_list);
// 	int i = 0;
// 	while (own_env[i] != NULL)
// 	{
// 		ft_printf("%s\n", own_env[i]);
// 		i++;
// 	}
// 	return (0);
// }
