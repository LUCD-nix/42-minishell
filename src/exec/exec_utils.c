/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 13:29:33 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/02 13:30:54 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	exec_abs_path(t_ast *node)
{
	if (access(node->command->path, X_OK) == 0)
		return (0);
	perror("minishell");
	return (1);
}

int	path_not_found(void)
{
	extern int	errno;

	errno = ENOENT;
	perror("minishell: no defined path");
	return (-1);
}

int	exec_get_path_loop(char **path_dirs, t_ast *node)
{
	char	*path;
	int		i;

	i = -1;
	while (path_dirs[++i])
	{
		path = ft_strjoin(path_dirs[i], "/");
		path = ft_strjoin_free_first(path, node->command->path);
		if (access(path, X_OK) == 0)
		{
			free(node->command->path);
			node->command->path = path;
			ft_free_tab(path_dirs);
			return (0);
		}
		free(path);
	}
	ft_free_tab(path_dirs);
	perror("Error: program not in PATH");
	return (-1);
}
