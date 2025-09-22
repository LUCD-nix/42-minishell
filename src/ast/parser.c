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

static t_ast *parse_single_redirection(t_parser *parser, t_list **env)
{
    t_token_type redir_type;
    t_ast *redir_node;

    redir_type = parser->current->type;
    advance(parser);
    if (!check(parser, T_WORD))
        return (error(parser, "Expected filename after redirection"), NULL);

    if (redir_type == T_REDIR_IN)
        redir_node = init_ast_node(NODE_REDIR_IN, env);
    else if (redir_type == T_REDIR_OUT)
        redir_node = init_ast_node(NODE_REDIR_OUT, env);
    else if (redir_type == T_APPEND)
        redir_node = init_ast_node(NODE_APPEND, env);
    else if (redir_type == T_HEREDOC)
        redir_node = init_ast_node(NODE_HEREDOC, env);
    else
        return (error(parser, "Unknown redirection type"), NULL);

    if (!redir_node)
        return (error(parser, "Memory allocation failed"), NULL);

    redir_node->filename = ft_strdup(parser->current->value);
    if (!redir_node->filename)
        return (free_ast(redir_node), error(parser, "Memory allocation failed"), NULL);

    advance(parser);
    return (redir_node);
}



// static int	has_input_redirections(t_ast *node)
// {
// 	while (node && (node->type == NODE_REDIR_OUT || node->type == NODE_APPEND
// 			|| node->type == NODE_REDIR_IN || node->type == NODE_HEREDOC))
// 	{
// 		if (node->type == NODE_REDIR_IN || node->type == NODE_HEREDOC)
// 			return (1);
// 		node = node->left;
// 	}
// 	return (0);
// }

// static int	count_output_redirections(t_ast *node)
// {
// 	int	count;
//
// 	count = 0;
// 	while (node && (node->type == NODE_REDIR_OUT || node->type == NODE_APPEND
// 			|| node->type == NODE_REDIR_IN || node->type == NODE_HEREDOC))
// 	{
// 		if (node->type == NODE_REDIR_OUT || node->type == NODE_APPEND)
// 			count++;
// 		node = node->left;
// 	}
// 	return (count);
// }
//
// static t_ast	*find_last_output_redir(t_ast *node)
// {
// 	t_ast	*last_output;
//
// 	last_output = NULL;
// 	while (node && (node->type == NODE_REDIR_OUT || node->type == NODE_APPEND
// 			|| node->type == NODE_REDIR_IN || node->type == NODE_HEREDOC))
// 	{
// 		if (node->type == NODE_REDIR_OUT || node->type == NODE_APPEND)
// 			last_output = node;
// 		node = node->left;
// 	}
// 	return (last_output);
// }
//
// static void	move_last_output_to_top(t_ast **root)
// {
// 	t_ast	*last_output;
// 	t_ast	*temp;
// 	t_ast	*prev;
//
// 	last_output = find_last_output_redir(*root);
// 	if (!last_output || last_output == *root)
// 		return ;
// 	temp = *root;
// 	prev = NULL;
// 	while (temp && temp != last_output)
// 	{
// 		prev = temp;
// 		temp = temp->left;
// 	}
// 	if (prev)
// 		prev->left = last_output->left;
// 	last_output->left = *root;
// 	*root = last_output;
// }

static t_ast	*reorganize_redirections(t_ast *node)
{
	if (!node)
		return (NULL);
	if (node->type != NODE_REDIR_IN && node->type != NODE_REDIR_OUT
		&& node->type != NODE_APPEND && node->type != NODE_HEREDOC)
		return (node);
	
	// Pour les redirections output multiples, garder l'ordre naturel
	// (la dernière parsée = la dernière dans la commande = celle qui gagne)
	return (node);
}

t_ast	*parse_expression(t_parser *parser, t_precedence precedence,
						t_list **env)
{
	t_ast			*left;
	t_token_type	op_type;
	t_ast			*redir;

	left = parse_primary(parser, env);
	if (!left)
		return (NULL);
	while (!at_end(parser) && precedence <= get_precedence(parser->current->type))
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

static t_ast	*collect_all_redirections(t_parser *parser, t_ast *cmd, t_list **env)
{
	t_ast	*redirection_chain;
	t_ast	*current_redir;
	t_ast	*new_redir;

	redirection_chain = NULL;
	current_redir = NULL;
	while (parser->current && (parser->current->type == T_REDIR_IN ||
			parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND ||
			parser->current->type == T_HEREDOC))
	{
		new_redir = parse_single_redirection(parser, env);
		if (!new_redir)
			return (redirection_chain ? free_ast(redirection_chain) : NULL, NULL);
		if (!redirection_chain)
		{
			redirection_chain = new_redir;
			current_redir = redirection_chain;
		}
		else
		{
			current_redir->left = new_redir;
			current_redir = new_redir;
		}
	}
	if (current_redir)
		current_redir->left = cmd;
	else
		return (cmd);
	return (redirection_chain);
}

static t_ast	*parse_command_with_redirections(t_parser *parser, t_list **env)
{
	t_ast	*cmd;
	t_ast	*result;
	t_ast	*prefix_redirs;

	prefix_redirs = NULL;
	if (parser->current && (parser->current->type == T_REDIR_IN ||
			parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND ||
			parser->current->type == T_HEREDOC))
	{
		prefix_redirs = collect_all_redirections(parser, NULL, env);
		if (!prefix_redirs)
			return (NULL);
	}
	if (!check(parser, T_WORD))
		return (prefix_redirs ? free_ast(prefix_redirs) : NULL,
			error(parser, "Expected command"), NULL);
	cmd = parse_command(parser, env);
	if (!cmd)
		return (prefix_redirs ? free_ast(prefix_redirs) : NULL, NULL);
	if (prefix_redirs)
	{
		t_ast *temp = prefix_redirs;
		while (temp->left && temp->left->type != NODE_CMD)
			temp = temp->left;
		temp->left = cmd;
		result = prefix_redirs;
	}
	else
		result = cmd;
	result = collect_all_redirections(parser, result, env);
	return (result);
}

t_ast	*parse_primary(t_parser *parser, t_list **env)
{
	if (check(parser, T_LPAREN))
		return (parse_subshell(parser, env));
	if (check(parser, T_WORD) || (parser->current && (parser->current->type == T_REDIR_IN ||
			parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND ||
			parser->current->type == T_HEREDOC)))
		return (parse_command_with_redirections(parser, env));
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
