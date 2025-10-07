/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traversal_heredoc.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:41:41 by lucorrei          #+#    #+#             */
/*   Updated: 2025/10/06 20:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <readline/readline.h>

static int	restore_stdin(void)
{
	int	new_stdin;

	new_stdin = open("/dev/tty", O_RDONLY);
	if (new_stdin == -1)
		return (-1);
	if (dup2(new_stdin, STDIN_FILENO) == -1)
	{
		close(new_stdin);
		return (-1);
	}
	close(new_stdin);
	return (0);
}

int	read_and_process_input(int tmp_file_write, t_ast *node)
{
	char	*line;
	char	*clean_delimiter;
	int		process_result;

	clean_delimiter = get_clean_delimiter(node->filename);
	if (!clean_delimiter)
		return (-1);
	setup_heredoc_signals();
	while (1)
	{
		line = readline("> ");
		if (g_signal_received == SIGINT)
		{
			if (restore_stdin() == -1)
				exit_and_free(node, EXIT_FAILURE, "heredoc: can't restore stdin");
			return (handle_signal_interruption(line, clean_delimiter));
		}
		process_result = process_heredoc_line_input(line, clean_delimiter,
				tmp_file_write, node);
		if (process_result != 1)
			return (process_result);
	}
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
		close(tmp_file_write);
		setup_interactive_signals();
		return (-1);
	}
	tmp_file_read = open_tmp_file(tmp_file_write);
	close(tmp_file_write);
	setup_interactive_signals();
	if (tmp_file_read == -1)
		exit_and_free(node, EXIT_FAILURE, "heredoc: error reopening tmp file");
	return (tmp_file_read);
}
