/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:40:10 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:40:34 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*expand_status(char *res, int *i, int last_status)
{
	char	*status;
	char	*new_res;

	status = ft_itoa(last_status);
	if (!status)
		return (res);
	new_res = ft_strjoin(res, status);
	free(status);
	free(res);
	(*i)++;
	return (new_res);
}

char	*expand_env_var(char *res, char *value, int *i, t_list *env)
{
	int		start;
	char	*key;
	char	*val;
	char	*new_res;

	start = *i;
	while (ft_isalnum(value[*i]) || value[*i] == '_')
		(*i)++;
	key = ft_substr(value, start, *i - start);
	if (!key)
		return (res);
	val = env_get(env, key);
	if (!val)
		val = ft_strdup("");
	new_res = ft_strjoin(res, val);
	free(key);
	free(res);
	return (new_res);
}

char	*append_char(char *res, char c)
{
	char	buf[2];
	char	*new_res;

	buf[0] = c;
	buf[1] = '\0';
	new_res = ft_strjoin(res, buf);
	free(res);
	return (new_res);
}
