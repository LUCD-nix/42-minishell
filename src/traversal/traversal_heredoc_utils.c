/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 14:22:31 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 14:34:06 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_file_desc	open_tmp_file(t_file_desc fd)
{
	char		buf[15];
	char		*tmp;
	char		*filepath;
	t_file_desc	res;

	ft_strlcpy(buf, "/proc/self/fd/", 15);
	tmp = ft_itoa(fd);
	if (tmp == NULL)
		return (-1);
	filepath = ft_strjoin_free_second(buf, tmp);
	if (filepath == NULL)
		return (-1);
	res = open(filepath, O_RDONLY);
	free(filepath);
	return (res);
}

char	*clean_line_copy(char *line)
{
	char	*line_copy;
	int		line_len;
	int		i;

	if (!line)
		return (NULL);
	line_copy = ft_strdup(line);
	if (!line_copy)
		return (NULL);
	line_len = ft_strlen(line_copy);
	i = line_len - 1;
	while (i >= 0 && (line_copy[i] == '\n' || line_copy[i] == '\r'))
	{
		line_copy[i] = '\0';
		i--;
		line_len--;
	}
	return (line_copy);
}

int	is_delimiter_line(char *line, char *delimiter)
{
	int		delim_len;
	int		line_len;
	char	*line_copy;
	int		result;

	if (!line || !delimiter)
		return (0);
	line_copy = clean_line_copy(line);
	if (!line_copy)
		return (0);
	delim_len = ft_strlen(delimiter);
	line_len = ft_strlen(line_copy);
	if (delim_len != line_len)
		result = 0;
	else
		result = (ft_strncmp(line_copy, delimiter, delim_len) == 0);
	free(line_copy);
	return (result);
}

int	should_expand_variables(t_ast *node)
{
	if (!node)
		return (1);
	if (node->heredoc_quoted)
		return (0);
	return (1);
}

char	*get_clean_delimiter(char *filename)
{
	if (!filename)
		return (NULL);
	return (ft_strdup(filename));
}
