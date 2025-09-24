/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:38:27 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/24 14:42:44 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// char	**collect_args(t_parser *parser, int *count)
// {
// 	char	**args;
// 	int		capacity;
// 	char	**new_args;
// 	int		i;
//
// 	*count = 0;
// 	capacity = 4;
// 	args = malloc(sizeof(char *) * capacity);
// 	if (!args)
// 		return (NULL);
// 	while (check(parser, T_WORD))
// 	{
// 		if (*count >= capacity - 1)
// 		{
// 			capacity *= 2;
// 			new_args = malloc(sizeof(char *) * capacity);
// 			if (!new_args)
// 				return (ft_free_tab(args), NULL);
// 			i = 0;
// 			while (i < *count)
// 			{
// 				new_args[i] = args[i];
// 				i++;
// 			}
// 			free(args);
// 			args = new_args;
// 		}
// 		args[*count] = ft_strdup(parser->current->value);
// 		if (!args[*count])
// 		{
// 			while (--(*count) >= 0)
// 				free(args[*count]);
// 			return (free(args), NULL);
// 		}
// 		(*count)++;
// 		advance(parser);
// 		if (parser->current
// 			&& get_precedence(parser->current->type) > PREC_NONE)
// 			break ;
// 	}
// 	args[*count] = NULL;
// 	return (args);
// }

t_ast	*parse_expression(t_parser *parser, t_precedence precedence,
		t_list **env)
{
	t_ast			*left;
	t_token_type	op_type;
	t_ast			*redir;

	left = parse_primary(parser, env);
	if (!left)
		return (NULL);
	while (!at_end(parser)
		&& precedence <= get_precedence(parser->current->type))
	{
		op_type = parser->current->type;
		if (op_type == T_REDIR_IN || op_type == T_REDIR_OUT
			|| op_type == T_APPEND || op_type == T_HEREDOC)
		{
			redir = parse_single_redirection(parser, env);
			if (redir)
			{
				redir->left = left;
				left = redir;
			}
		}
		else if (op_type == T_PIPE || op_type == T_AND || op_type == T_OR)
		{
			advance(parser);
			left = parse_binary(parser, left, op_type, env);
		}
		else
			break ;
		if (!left)
			break ;
	}
	left = reorganize_redirections(left);
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
