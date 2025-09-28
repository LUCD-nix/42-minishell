/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:41:41 by lucorrei          #+#    #+#             */
/*   Updated: 2025/09/28 14:34:23 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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

void	write_processed_line(int fd, char *line, t_ast *node)
{
	char	*processed_line;

	processed_line = process_heredoc_line(line, node);
	if (processed_line)
	{
		write(fd, processed_line, ft_strlen(processed_line));
		if (processed_line != line)
			free(processed_line);
	}
}

static int	process_input_line(int tmp_file_write, char *line, t_ast *node)
{
	char	*original_line;

	original_line = line;
	write_processed_line(tmp_file_write, line, node);
	free(original_line);
	return (0);
}

int	read_and_process_input(int tmp_file_write, t_ast *node)
{
	char	*line;
	char	*clean_delimiter;

	clean_delimiter = get_clean_delimiter(node->filename);
	if (!clean_delimiter)
		return (-1);
	line = get_next_line(STDIN_FILENO);
	while (line && !is_delimiter_line(line, clean_delimiter))
	{
		process_input_line(tmp_file_write, line, node);
		line = get_next_line(STDIN_FILENO);
	}
	if (line == NULL)
	{
		free(clean_delimiter);
		exit_and_free(node, EXIT_FAILURE, "heredoc: stdin read error");
	}
	free(line);
	free(clean_delimiter);
	return (0);
}

int	traverse_heredoc(t_ast *node)
{
	int	tmp_file_read;
	int	tmp_file_write;

	if (!node || !node->filename)
		return (-1);
	tmp_file_write = open("/var/tmp", O_RDWR | __O_TMPFILE, 0644);
	if (tmp_file_write == -1)
		exit_and_free(node, EXIT_FAILURE, "heredoc: tmp file error");
	read_and_process_input(tmp_file_write, node);
	tmp_file_read = open_tmp_file(tmp_file_write);
	close(tmp_file_write);
	if (tmp_file_read == -1)
		exit_and_free(node, EXIT_FAILURE, "heredoc: error reopening tmp file");
	return (tmp_file_read);
}
