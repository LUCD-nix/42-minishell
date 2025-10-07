/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traversal_heredoc_utils4.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 13:49:21 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/07 00:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	handle_heredoc_eof(char *line, char *delimiter)
{
	if (!line)
	{
		ft_printf("minishell: warning: here-document delimited by");
		ft_printf(" end-of-file (wanted `%s')\n", delimiter);
		return (1);
	}
	return (0);
}

int	handle_signal_interruption(char *line, char *delimiter)
{
	if (line)
		free(line);
	free(delimiter);
	g_signal_received = 0;
	return (-1);
}

int	process_heredoc_line_input(char *line, char *delimiter,
		int tmp_fd, t_ast *node)
{
	if (handle_heredoc_eof(line, delimiter))
		return (free(delimiter), 0);
	if (check_delimiter_and_process(line, delimiter, tmp_fd, node))
		return (free(delimiter), 0);
	return (1);
}

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
