/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc_utils3.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 14:33:25 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 14:33:58 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	expand_variable(t_heredoc_expand *data)
{
	char	*var_name;
	char	*var_value;

	(*data->i)++;
	var_name = extract_var_name(data->line, *data->i,
			get_var_name_len(data->line, *data->i));
	if (var_name)
	{
		var_value = get_env_var_value(var_name, data->env);
		if (var_value)
			copy_var_value(data->result, data->j, var_value);
		*data->i += get_var_name_len(data->line, *data->i);
		free(var_name);
	}
	else
		data->result[(*data->j)++] = data->line[(*data->i)++];
}

void	process_character(t_heredoc_expand *data)
{
	if (data->line[*data->i] == '$' && data->line[*data->i + 1]
		&& (ft_isalnum(data->line[*data->i + 1]) || data->line[*data->i
			+ 1] == '_'))
		expand_variable(data);
	else
		data->result[(*data->j)++] = data->line[(*data->i)++];
}

char	*expand_line_variables(char *line, t_list **env)
{
	char				*result;
	int					i;
	int					j;
	t_heredoc_expand	data;

	if (!line)
		return (NULL);
	result = malloc(ft_strlen(line) * (2 + 1));
	if (!result)
		return (line);
	i = 0;
	j = 0;
	data = (t_heredoc_expand){line, result, &i, &j, env};
	while (line[i])
		process_character(&data);
	result[j] = '\0';
	return (result);
}
