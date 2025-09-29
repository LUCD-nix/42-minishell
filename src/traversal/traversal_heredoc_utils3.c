/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc_utils3.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 14:33:25 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 14:57:07 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	expand_variable(t_heredoc_expand *data)
{
	char	*var_name;
	char	*var_value;
	int		var_len;

	(*data->i)++;
	var_len = get_var_name_len(data->line, *data->i);
	if (var_len == 0)
	{
		data->result[(*data->j)++] = '$';
		return ;
	}
	var_name = extract_var_name(data->line, *data->i, var_len);
	if (var_name)
	{
		var_value = get_env_var_value(var_name, data->env);
		if (var_value)
			copy_var_value(data->result, data->j, var_value);
		*data->i += var_len;
		free(var_name);
	}
	else
		data->result[(*data->j)++] = data->line[(*data->i)++];
}

void	process_character_bis(t_heredoc_expand *data)
{
	if (data->line[*data->i] == '$' && data->line[*data->i + 1]
		&& (ft_isalnum(data->line[*data->i + 1])
			|| data->line[*data->i + 1] == '_'))
		expand_variable(data);
	else
		data->result[(*data->j)++] = data->line[(*data->i)++];
}

static size_t	calc_expand_size(char *line, t_list **env, int i)
{
	size_t	size;
	char	*var_name;
	char	*var_value;
	int		var_len;

	size = 0;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1]
			&& (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			i++;
			var_len = get_var_name_len(line, i);
			var_name = extract_var_name(line, i, var_len);
			var_value = get_env_var_value(var_name, env);
			if (var_value)
				size += ft_strlen(var_value);
			free(var_name);
			i += var_len;
		}
		else
		{
			size++;
			i++;
		}
	}
	return (size);
}

char	*expand_line_variables(char *line, t_list **env)
{
	char				*result;
	int					i;
	int					j;
	t_heredoc_expand	data;
	size_t				result_size;

	if (!line)
		return (NULL);
	result_size = calc_expand_size(line, env, 0);
	result = malloc(result_size + 1);
	if (!result)
		return (ft_strdup(line));
	i = 0;
	j = 0;
	data = (t_heredoc_expand){line, result, &i, &j, env};
	while (line[i])
		process_character_bis(&data);
	result[j] = '\0';
	return (result);
}

char	*process_heredoc_line(char *line, t_ast *node)
{
	char	*expanded_line;

	if (!line)
		return (NULL);
	if (should_expand_variables(node) && contains_variables(line))
	{
		expanded_line = expand_line_variables(line, node->env);
		if (expanded_line && expanded_line != line)
			return (expanded_line);
	}
	return (line);
}
