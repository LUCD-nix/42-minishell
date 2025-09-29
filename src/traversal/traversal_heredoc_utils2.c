/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traverse_heredoc_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 14:22:42 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 14:50:34 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	contains_variables(char *line)
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

int	get_var_name_len(char *line, int start)
{
	int	i;

	i = 0;
	while (line[start + i] && (ft_isalnum(line[start + i])
			|| line[start + i] == '_'))
		i++;
	return (i);
}

char	*extract_var_name(char *line, int start, int len)
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

char	*get_env_var_value(char *var_name, t_list **env)
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

void	copy_var_value(char *result, int *j, char *var_value)
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
