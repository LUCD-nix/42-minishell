/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:26:42 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 14:00:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_filename	init_filename_result(t_parser *parser)
{
	t_filename	result;

	result.value = ft_strdup(parser->current->value);
	if (!result.value)
		return ((t_filename){NULL, 0});
	result.has_quotes = (parser->current->quote != Q_NONE);
	advance(parser);
	return (result);
}

int	contains_quotes(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

t_filename	concat_quoted_tokens(t_parser *parser)
{
	t_filename	result;
	char		*temp;
	char		*cleaned;

	result = init_filename_result(parser);
	if (!result.value)
		return (result);
	if (contains_quotes(result.value))
		result.has_quotes = 1;
	while (check(parser, T_WORD) && parser->current)
	{
		if (parser->current->quote != Q_NONE
			|| contains_quotes(parser->current->value))
			result.has_quotes = 1;
		temp = ft_strjoin(result.value, parser->current->value);
		free(result.value);
		if (!temp)
			return ((t_filename){NULL, 0});
		result.value = temp;
		advance(parser);
	}
	cleaned = remove_quotes(result.value);
	free(result.value);
	if (!cleaned)
		return ((t_filename){NULL, 0});
	return (result.value = cleaned, result);
}

t_ast	*parse_single_redirection(t_parser *parser, t_list **env)
{
	t_token_type	redir_type;
	t_ast			*redir_node;
	t_filename		fname;

	redir_type = parser->current->type;
	advance(parser);
	if (!check(parser, T_WORD))
		return (error(parser, "Expected filename after redirection"), NULL);
	fname = concat_quoted_tokens(parser);
	if (!fname.value)
		return (error(parser, "Memory allocation failed"), NULL);
	redir_node = create_redirection_node(redir_type, env);
	if (!redir_node)
	{
		free(fname.value);
		return (error(parser, "Unknown redirection type"), NULL);
	}
	if (redir_node->type == NODE_HEREDOC)
		redir_node->heredoc_quoted = fname.has_quotes;
	redir_node->filename = fname.value;
	return (redir_node);
}

int	is_redirection_token(t_parser *parser)
{
	return (parser->current && (parser->current->type == T_REDIR_IN
			|| parser->current->type == T_REDIR_OUT
			|| parser->current->type == T_APPEND
			|| parser->current->type == T_HEREDOC));
}
