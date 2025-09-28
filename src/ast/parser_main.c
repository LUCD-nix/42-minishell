/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:27 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 13:16:42 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*parse_expression(t_parser *parser, t_precedence precedence,
		t_list **env)
{
	t_ast			*left;
	t_token_type	op_type;

	left = parse_primary(parser, env);
	if (!left)
		return (NULL);
	while (!at_end(parser)
		&& precedence <= get_precedence(parser->current->type))
	{
		op_type = parser->current->type;
		left = process_operator(parser, left, op_type, env);
		if (!left)
			break ;
		if (!is_redirection_operator(op_type) && !is_binary_operator(op_type))
			break ;
	}
	return (left);
}

void	propagate_first_node(t_ast *first, t_ast *current)
{
	if (current == NULL)
		return ;
	current->top = first;
	propagate_first_node(first, current->left);
	propagate_first_node(first, current->right);
}

t_ast	*parse(t_token *tokens, t_list **env)
{
	t_parser	parser;
	t_ast		*result;

	parser.tokens = tokens;
	parser.current = tokens;
	parser.had_error = 0;
	if (at_end(&parser))
		return (NULL);
	result = parse_expression(&parser, PREC_NONE, env);
	if (parser.had_error || !at_end(&parser))
	{
		if (result)
			free_ast(result);
		return (NULL);
	}
	propagate_first_node(result, result);
	return (result);
}
