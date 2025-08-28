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
#include "../../minishell.h"

void	env_free(void *ptr)
{
	t_env	*env;

	env = (t_env *) ptr;
	free(env->key);
	free(env->value);
	free(env);
}

void	env_delete_key(t_list **head, char *key)
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
		return ;
	prev->next = temp->next;
	ft_lstdelone(temp, &env_free);
}

char	*env_get(t_list *env, char *key)
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
