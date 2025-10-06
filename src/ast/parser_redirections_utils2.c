/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections_utils2.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 17:44:43 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 17:44:45 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*create_redirection_node(t_token_type redir_type, t_list **env)
{
	t_ast	*redir_node;

	if (redir_type == T_REDIR_IN)
		redir_node = init_ast_node(NODE_REDIR_IN, env);
	else if (redir_type == T_REDIR_OUT)
		redir_node = init_ast_node(NODE_REDIR_OUT, env);
	else if (redir_type == T_APPEND)
		redir_node = init_ast_node(NODE_APPEND, env);
	else if (redir_type == T_HEREDOC)
	{
		redir_node = init_ast_node(NODE_HEREDOC, env);
		if (redir_node)
			redir_node->heredoc_quoted = 0;
	}
	else
		return (NULL);
	return (redir_node);
}

int	process_quote_section(char *str, int *i, char *result, int *j)
{
	char	quote;

	quote = str[*i];
	(*i)++;
	while (str[*i] && str[*i] != quote)
	{
		result[*j] = str[*i];
		(*j)++;
		(*i)++;
	}
	if (str[*i] == quote)
		(*i)++;
	return (0);
}

char	*remove_quotes(char *str)
{
	char	*result;
	int		i;
	int		j;

	if (!str)
		return (NULL);
	result = ft_calloc(ft_strlen(str) + 1, 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			process_quote_section(str, &i, result, &j);
		else
		{
			result[j] = str[i];
			j++;
			i++;
		}
	}
	result[j] = '\0';
	return (result);
}
