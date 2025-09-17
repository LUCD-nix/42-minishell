/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:27:01 by lucorrei          #+#    #+#             */
/*   Updated: 2025/08/28 15:27:04 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_ast	*parse_binary(t_parser *parser, t_ast *left, t_token_type op,
						t_list **env)
{
	t_ast		*node;
	t_node_type	node_type;

	if (op == T_PIPE)
		node_type = NODE_PIPE;
	else if (op == T_AND)
		node_type = NODE_AND;
	else if (op == T_OR)
		node_type = NODE_OR;
	else
		return (error(parser, "Unknown binary operator"), left);
	node = init_ast_node(node_type, env);
	if (!node)
		return (error(parser, "Memory allocation failed"), left);
	node->left = left;
	node->right = parse_expression(parser, get_precedence(op) + 1, env);
	if (!node->right)
		return (free_ast(node), NULL);
	return (node);
}

static t_ast	*parse_redirection(t_parser *parser, t_ast *left, t_list **env)
{
	t_token_type	redir_type;
	t_ast			*redir_node;

	redir_type = parser->current->type;
	advance(parser);
	if (!check(parser, T_WORD))
		return (error(parser, "Expected filename after redirection"), left);
	if (redir_type == T_REDIR_IN)
		redir_node = init_ast_node(NODE_REDIR_IN, env);
	else if (redir_type == T_REDIR_OUT)
		redir_node = init_ast_node(NODE_REDIR_OUT, env);
	else if (redir_type == T_APPEND)
		redir_node = init_ast_node(NODE_APPEND, env);
	else if (redir_type == T_HEREDOC)
		redir_node = init_ast_node(NODE_HEREDOC, env);
	else
		return (error(parser, "Unknown redirection type"), left);
	if (!redir_node)
		return (error(parser, "Memory allocation failed"), left);
	redir_node->filename = ft_strdup(parser->current->value);
	if (!redir_node->filename)
		return (free_ast(redir_node), error(parser, "Memory allocation failed"), left);
	redir_node->left = left;
	advance(parser);
	return (redir_node);
}

t_ast	*parse_expression(t_parser *parser, t_precedence precedence,
						t_list **env)
{
	t_ast			*left;
	t_token_type	op_type;

	left = parse_primary(parser, env);
	if (!left)
		return (NULL);
	while (!at_end(parser) && precedence <= get_precedence(parser->current->type))
	{
		op_type = parser->current->type;
		if (op_type == T_REDIR_IN || op_type == T_REDIR_OUT
			|| op_type == T_APPEND || op_type == T_HEREDOC)
		{
			left = parse_redirection(parser, left, env);
		}
		else if (op_type == T_PIPE || op_type == T_AND || op_type == T_OR)
		{
			advance(parser);
			left = parse_binary(parser, left, op_type, env);
		}
		else
		{
			break ;
		}
		if (!left)
			break ;
	}
	return (left);
}

t_ast	*parse_primary(t_parser *parser, t_list **env)
{
	if (check(parser, T_LPAREN))
		return (parse_subshell(parser, env));
	if (check(parser, T_WORD))
		return (parse_command(parser, env));
	error(parser, "Expected command or '('");
	return (NULL);
}

t_ast	*parse_subshell(t_parser *parser, t_list **env)
{
	t_ast	*node;

	advance(parser);
	node = init_ast_node(NODE_SUBSHELL, env);
	if (!node)
		return (error(parser, "Memory allocation failed"), NULL);
	node->left = parse_expression(parser, PREC_NONE, env);
	if (!node->left)
	{
		error(parser, "Expected expression in subshell");
		return (free_ast(node), NULL);
	}
	if (!check(parser, T_RPAREN))
	{
		error(parser, "Expected ')' after subshell");
		return (free_ast(node), NULL);
	}
	advance(parser);
	return (node);
}

static char	**collect_args(t_parser *parser, int *count)
{
	char	**args;
	int		capacity;
	char	**new_args;
	int		i;

	*count = 0;
	capacity = 4;
	args = malloc(sizeof(char *) * capacity);
	if (!args)
		return (NULL);
	while (check(parser, T_WORD))
	{
		if (*count >= capacity - 1)
		{
			capacity *= 2;
			new_args = malloc(sizeof(char *) * capacity);
			if (!new_args)
				return (ft_free_tab(args), NULL);
			i = 0;
			while (i < *count)
			{
				new_args[i] = args[i];
				i++;
			}
			free(args);
			args = new_args;
		}
		args[*count] = ft_strdup(parser->current->value);
		if (!args[*count])
		{
			while (--(*count) >= 0)
				free(args[*count]);
			return (free(args), NULL);
		}
		(*count)++;
		advance(parser);
		if (parser->current && get_precedence(parser->current->type) > PREC_NONE)
			break ;
	}
	args[*count] = NULL;
	return (args);
}

t_ast	*parse_command(t_parser *parser, t_list **env)
{
	char		**args;
	int			argc;
	t_command	*cmd;
	t_ast		*node;

	args = collect_args(parser, &argc);
	if (!args || argc == 0)
		return (error(parser, "Expected command"), NULL);
	cmd = init_cmd(args);
	ft_free_tab(args);
	if (!cmd)
		return (error(parser, "Memory allocation failed"), NULL);
	node = init_cmd_node(cmd, env);
	if (!node)
		return (free_cmd(cmd), error(parser, "Memory allocation failed"), NULL);
	return (node);
}

void	propagate_first_node(t_ast	*first, t_ast *current)
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

