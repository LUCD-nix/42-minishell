/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:26:42 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 18:45:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*join_token_value(char *current, char *to_add)
{
	char	*temp;

	temp = ft_strjoin(current, to_add);
	free(current);
	return (temp);
}

t_filename	concat_loop(t_parser *parser, t_filename result)
{
	char	*temp;

	while (check(parser, T_WORD) && parser->current)
	{
		update_quotes_flag(parser, &result);
		temp = join_token_value(result.value, parser->current->value);
		if (!temp)
			return ((t_filename){NULL, 0});
		result.value = temp;
		advance(parser);
	}
	return (result);
}

t_filename	concat_quoted_tokens(t_parser *parser)
{
	t_filename	result;
	char		*cleaned;

	result = init_filename_result(parser);
	if (!result.value)
		return (result);
	if (contains_quotes(result.value))
		result.has_quotes = 1;
	result = concat_loop(parser, result);
	if (!result.value)
		return ((t_filename){NULL, 0});
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
