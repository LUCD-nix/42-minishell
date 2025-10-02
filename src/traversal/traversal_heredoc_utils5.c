/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traversal_heredoc_utils5.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:00:38 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/02 14:01:03 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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

int	process_input_line(int tmp_file_write, char *line, t_ast *node)
{
	write_processed_line(tmp_file_write, line, node);
	write(tmp_file_write, "\n", 1);
	return (0);
}

int	check_delimiter_and_process(char *line, char *delimiter,
		int tmp_fd, t_ast *node)
{
	if (is_delimiter_line(line, delimiter))
	{
		free(line);
		return (1);
	}
	process_input_line(tmp_fd, line, node);
	free(line);
	return (0);
}
