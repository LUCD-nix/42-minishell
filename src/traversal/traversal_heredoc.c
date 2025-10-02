/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:41:41 by lucorrei          #+#    #+#             */
/*   Updated: 2025/09/28 14:55:24 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <readline/readline.h>

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

static int	check_delimiter_and_process(char *line, char *delimiter,
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

static int	handle_heredoc_eof(char *line, char *delimiter)
{
	if (!line)
	{
		ft_printf("minishell: warning: here-document delimited by");
		ft_printf(" end-of-file (wanted `%s')\n", delimiter);
		return (1);
	}
	return (0);
}

int	read_and_process_input(int tmp_file_write, t_ast *node)
{
	char	*line;
	char	*clean_delimiter;

	clean_delimiter = get_clean_delimiter(node->filename);
	if (!clean_delimiter)
		return (-1);
	setup_heredoc_signals();
	while (1)
	{
		line = readline("> ");
		if (g_signal_received == SIGINT)
		{
			if (line)
				free(line);
			free(clean_delimiter);
			g_signal_received = 0;
			return (-1);
		}
		if (handle_heredoc_eof(line, clean_delimiter))
			return (free(clean_delimiter), 0);
		if (check_delimiter_and_process(line, clean_delimiter,
				tmp_file_write, node))
			break ;
	}
	return (free(clean_delimiter), 0);
}

int	traverse_heredoc(t_ast *node)
{
	int	tmp_file_read;
	int	tmp_file_write;
	int	read_status;

	if (!node || !node->filename)
		return (-1);
	tmp_file_write = open("/var/tmp", O_RDWR | __O_TMPFILE, 0644);
	if (tmp_file_write == -1)
		exit_and_free(node, EXIT_FAILURE, "heredoc: tmp file error");
	read_status = read_and_process_input(tmp_file_write, node);
	if (read_status == -1)
	{
		return (close(tmp_file_write), setup_interactive_signals(),
			rl_on_new_line(), rl_redisplay(),-1);
	}
	tmp_file_read = open_tmp_file(tmp_file_write);
	close(tmp_file_write);
	setup_interactive_signals();
	if (tmp_file_read == -1)
		exit_and_free(node, EXIT_FAILURE, "heredoc: error reopening tmp file");
	return (tmp_file_read);
}
