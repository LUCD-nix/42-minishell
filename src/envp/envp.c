/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:36:19 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 14:57:01 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*handle_dollar_expansion(char *res, char *value, int *i,
		t_expansion_data *data)
{
	(*i)++;
	if (value[*i] == '?')
		return (expand_status(res, i, data->last_status));
	else if (ft_isalnum(value[*i]) || value[*i] == '_')
		return (expand_env_var(res, value, i, data->envp));
	else
		return (append_char(res, '$'));
}

static char	*process_character(char *res, char *value, int *i,
		t_expansion_data *data)
{
	if (value[*i] == '$')
		return (handle_dollar_expansion(res, value, i, data));
	else
		return (append_char(res, value[(*i)++]));
}

static char	*init_expansion_result(char *value, t_quote_type quote)
{
	if (!value)
		return (NULL);
	if (quote == Q_SIMPLE)
		return (ft_strdup(value));
	return (ft_strdup(""));
}

char	*expand_variables(char *value, t_list *envp, int last_status,
		t_quote_type quote)
{
	char				*res;
	int					i;
	t_expansion_data	data;

	res = init_expansion_result(value, quote);
	if (!res || quote == Q_SIMPLE)
		return (res);
	data.envp = envp;
	data.last_status = last_status;
	i = 0;
	while (value[i])
	{
		res = process_character(res, value, &i, &data);
		if (!res)
			return (NULL);
	}
	return (res);
}
