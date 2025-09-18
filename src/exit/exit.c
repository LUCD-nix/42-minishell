/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 16:01:13 by lucorrei          #+#    #+#             */
/*   Updated: 2025/09/17 16:01:15 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

void	exit_and_free(t_ast *node, int exit_code, char *message)
{
	if (exit_code != EXIT_SUCCESS && message != NULL)
	{
		ft_printf("minishell: ");
		perror(message);
	}
	ft_lstclear(node->env, &env_free);
	free_ast(node->top);
	exit(exit_code);
}
