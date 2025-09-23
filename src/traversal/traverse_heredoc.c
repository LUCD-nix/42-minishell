/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:41:41 by lucorrei          #+#    #+#             */
/*   Updated: 2025/09/22 17:41:46 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

static t_file_desc	open_tmp_file(t_file_desc fd)
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

int	traverse_heredoc(t_ast *node)
{
	int		tmp_file_read;
	int		tmp_file_write;
	char	*line;
	int		delim_len;

	delim_len = ft_strlen(node->filename);
	tmp_file_write = open("/var/tmp", O_RDWR | __O_TMPFILE, 0644);
	if (tmp_file_write == -1)
		exit_and_free(node, EXIT_FAILURE, "heredoc: tmp file error");
	line = get_next_line(STDIN_FILENO);
	while (ft_strncmp(line, node->filename, delim_len + 1) != '\n')
	{
		if (line == NULL)
			exit_and_free(node, EXIT_FAILURE, "heredoc: stdin read error");
		write(tmp_file_write, line, ft_strlen(line));
		free(line);
		line = get_next_line(STDIN_FILENO);
	}
	free(line);
	tmp_file_read = open_tmp_file(tmp_file_write);
	close(tmp_file_write);
	if (tmp_file_read == -1)
		exit_and_free(node, EXIT_FAILURE, "heredoc: error reopening tmp file");
	return (tmp_file_read);
}
