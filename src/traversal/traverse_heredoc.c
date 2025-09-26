/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:41:41 by lucorrei          #+#    #+#             */
/*   Updated: 2025/09/25 00:00:00 by lucorrei         ###   ########.fr       */
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

static int	is_delimiter_line(char *line, char *delimiter)
{
	int		delim_len;
	int		line_len;
	char	*line_copy;
	int		result;
	int		i;

	if (!line || !delimiter)
		return (0);
	line_copy = ft_strdup(line);
	if (!line_copy)
		return (0);
	delim_len = ft_strlen(delimiter);
	line_len = ft_strlen(line_copy);
	i = line_len - 1;
	while (i >= 0 && (line_copy[i] == '\n' || line_copy[i] == '\r'))
	{
		line_copy[i] = '\0';
		i--;
		line_len--;
	}
	if (delim_len != line_len)
		result = 0;
	else
		result = (ft_strncmp(line_copy, delimiter, delim_len) == 0);
	free(line_copy);
	return (result);
}

static int	should_expand_variables(t_ast *node)
{
	if (!node)
		return (1);
	if (node->heredoc_quoted)
		return (0);
	return (1);
}

static char	*get_clean_delimiter(char *filename)
{
	if (!filename)
		return (NULL);
	return (ft_strdup(filename));
}

static int	contains_variables(char *line)
{
	int	i;

	if (!line)
		return (0);
	i = 0;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1] 
			&& (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
			return (1);
		i++;
	}
	return (0);
}

static int	get_var_name_len(char *line, int start)
{
	int	i;

	i = 0;
	while (line[start + i] && (ft_isalnum(line[start + i]) 
			|| line[start + i] == '_'))
		i++;
	return (i);
}

static char	*extract_var_name(char *line, int start, int len)
{
	char	*var_name;
	int		i;

	var_name = malloc(len + 1);
	if (!var_name)
		return (NULL);
	i = 0;
	while (i < len)
	{
		var_name[i] = line[start + i];
		i++;
	}
	var_name[i] = '\0';
	return (var_name);
}

static char	*get_env_var_value(char *var_name, t_list **env)
{
	t_list	*current;
	t_env	*env_entry;
	size_t	var_len;

	if (!var_name || !env || !*env)
		return (NULL);
	current = *env;
	var_len = ft_strlen(var_name);
	while (current)
	{
		env_entry = (t_env *)current->content;
		if (ft_strncmp(env_entry->key, var_name, var_len) == 0 
			&& env_entry->key[var_len] == '\0')
			return (env_entry->value);
		current = current->next;
	}
	return (NULL);
}

static void	copy_var_value(char *result, int *j, char *var_value)
{
	int	k;

	if (!var_value)
		return ;
	k = 0;
	while (var_value[k])
	{
		result[*j] = var_value[k];
		(*j)++;
		k++;
	}
}

static char	*expand_line_variables(char *line, t_list **env)
{
	char	*result;
	char	*var_name;
	char	*var_value;
	int		i;
	int		j;

	if (!line)
		return (NULL);
	result = malloc(ft_strlen(line) * (2 + 1));
	if (!result)
		return (line);
	i = 0;
	j = 0;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1] 
			&& (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			i++;
			var_name = extract_var_name(line, i, get_var_name_len(line, i));
			if (var_name)
			{
				var_value = get_env_var_value(var_name, env);
				if (var_value)
					copy_var_value(result, &j, var_value);
				i += get_var_name_len(line, i);
				free(var_name);
			}
			else
				result[j++] = line[i++];
		}
		else
			result[j++] = line[i++];
	}
	result[j] = '\0';
	return (result);
}

static char	*process_heredoc_line(char *line, t_ast *node)
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

static void	write_processed_line(int fd, char *line, t_ast *node)
{
	char	*processed_line;

	processed_line = process_heredoc_line(line, node);
	if (processed_line)
	{
		write(fd, processed_line, ft_strlen(processed_line));
		if (processed_line != line)
		{
			free(processed_line);
		}
	}
}

static int	read_and_process_input(int tmp_file_write, t_ast *node)
{
	char	*line;
	char	*original_line;
	char	*clean_delimiter;

	clean_delimiter = get_clean_delimiter(node->filename);
	if (!clean_delimiter)
		return (-1);
	line = get_next_line(STDIN_FILENO);
	while (line && !is_delimiter_line(line, clean_delimiter))
	{
		original_line = line;
		write_processed_line(tmp_file_write, line, node);
		free(original_line);
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
		exit_and_free(node, EXIT_FAILURE, 
			"heredoc: error reopening tmp file");
	return (tmp_file_read);
}
