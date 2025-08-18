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

int	env_remove_key(t_env *env, char *key)
{
	int	i;
	int	key_length;

	key_length = ft_strlen(key);
	i = 0;
	while (i < env->size)
	{
		if (!ft_strncmp(key, env->contents[i], key_length))
		{
			free(env->contents[i]);
			env->contents[i] = env->contents[env->size - 1];
			env->contents[env->size - 1] = NULL;
			return (0);
		}
		i++;
	}
	return (-1);
}

t_env	*env_from_str_arr(char **to_copy)
{
	t_env		*res;
	int			len;
	int			i;

	len = 0;
	while (to_copy[len] != NULL)
		len++;
	res = ft_calloc(1, sizeof(*res) + 2 * len * sizeof(char *) + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (i < len)
	{
		res->contents[i] = ft_strdup(to_copy[i]);
		if (res->contents[i] == NULL)
		{
			free(res);
			// TODO : print error ?
			exit(EXIT_FAILURE);
		}
		i++;
	}
	res->capacity = 2 * len;
	res->size = len;
	return (res);
}

t_env	*env_add(t_env *arr, char *str)
{
	t_env	*temp;

	if (arr->capacity >= arr->size + 1)
	{
		arr->contents[arr->size] = str;
		arr->size++;
	}
	else
	{
		temp = env_from_str_arr(arr->contents);
		free(arr);
		arr = temp;
		env_add(arr, str);	// TODO : testing
	}
	return (arr);
}
