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

static t_ast	*create_redirection_node(t_parser *parser, t_token_type redir_type, t_list **env)
{
	t_ast	*redir_node;

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

	if (!check(parser, T_WORD))
		return (free_ast(redir_node), error(parser, "Expected filename after redirection"), NULL);

	redir_node->filename = ft_strdup(parser->current->value);
	if (!redir_node->filename)
		return (free_ast(redir_node), error(parser, "Memory allocation failed"), NULL);

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
		
		// Les redirections sont maintenant gérées dans parse_command/parse_primary
		if (op_type == T_REDIR_IN || op_type == T_REDIR_OUT
			|| op_type == T_APPEND || op_type == T_HEREDOC)
		{
			// Ces redirections ont déjà été traitées dans parse_command
			break;
		}
		else if (op_type == T_PIPE || op_type == T_AND || op_type == T_OR)
		{
			advance(parser);
			left = parse_binary(parser, left, op_type, env);
		}
		else
		{
			break;
		}
		if (!left)
			break;
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


static int	handle_output_redirection(t_parser *parser, t_ast *cmd)
{
	t_token_type	redir_type;

	redir_type = parser->current->type;
	advance(parser);
	
	if (!check(parser, T_WORD))
	{
		error(parser, "Expected filename after redirection");
		return (-1);
	}
	
	// Libérer l'ancien fichier de sortie s'il existe
	if (cmd->output_file)
	{
		free(cmd->output_file);
		cmd->output_file = NULL;
	}
	
	// Assigner le nouveau fichier de sortie
	cmd->output_file = ft_strdup(parser->current->value);
	if (!cmd->output_file)
	{
		error(parser, "Memory allocation failed");
		return (-1);
	}
	
	// Définir le mode (> ou >>)
	cmd->append_mode = (redir_type == T_APPEND) ? 1 : 0;
	
	advance(parser);
	return (0);
}

static int	apply_redirections_to_subshell(t_parser *parser, t_ast *subshell)
{
	while (!at_end(parser) && (parser->current->type == T_REDIR_IN || 
			parser->current->type == T_REDIR_OUT || 
			parser->current->type == T_APPEND || 
			parser->current->type == T_HEREDOC))
	{
		if (parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND)
		{
			if (handle_output_redirection(parser, subshell) == -1)
				return (-1);
		}
		else if (parser->current->type == T_REDIR_IN)
		{
			if (handle_input_redirection(parser, subshell) == -1)
				return (-1);
		}
		else if (parser->current->type == T_HEREDOC)
		{
			if (handle_heredoc(parser, subshell) == -1)
				return (-1);
		}
	}
	return (0);
}

t_ast	*parse_subshell(t_parser *parser, t_list **env)
{
	t_ast	*node;

	advance(parser); // Passer '('
	
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
	advance(parser); // Passer ')'
	
	// NOUVEAU: Appliquer les redirections au subshell
	if (apply_redirections_to_subshell(parser, node) == -1)
		return (free_ast(node), NULL);
	
	return (node);
}

static int	handle_input_redirection(t_parser *parser, t_ast *cmd)
{
	advance(parser);
	
	if (!check(parser, T_WORD))
	{
		error(parser, "Expected filename after redirection");
		return (-1);
	}
	
	// Libérer l'ancien fichier d'entrée s'il existe
	if (cmd->input_file)
	{
		free(cmd->input_file);
		cmd->input_file = NULL;
	}
	
	// Assigner le nouveau fichier d'entrée
	cmd->input_file = ft_strdup(parser->current->value);
	if (!cmd->input_file)
	{
		error(parser, "Memory allocation failed");
		return (-1);
	}
	
	advance(parser);
	return (0);
}

static int	handle_heredoc(t_parser *parser, t_ast *cmd)
{
	char	*delimiter;
	char	*line;
	int		pipe_fd[2];
	size_t	delim_len;

	advance(parser);
	
	if (!check(parser, T_WORD))
	{
		error(parser, "Expected delimiter after <<");
		return (-1);
	}
	
	delimiter = parser->current->value;
	delim_len = ft_strlen(delimiter);
	advance(parser);
	
	// Créer le pipe pour heredoc
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	
	// Lire les lignes jusqu'au délimiteur
	printf("heredoc> ");
	line = get_next_line(STDIN_FILENO);
	while (line)
	{
		if (ft_strlen(line) == delim_len + 1 && 
			ft_strncmp(line, delimiter, delim_len) == 0 && 
			line[delim_len] == '\n')
		{
			free(line);
			break;
		}
		write(pipe_fd[1], line, ft_strlen(line));
		free(line);
		printf("heredoc> ");
		line = get_next_line(STDIN_FILENO);
	}
	
	close(pipe_fd[1]);
	
	// Fermer l'ancien heredoc_fd s'il existe
	if (cmd->heredoc_fd >= 0)
		close(cmd->heredoc_fd);
	
	cmd->heredoc_fd = pipe_fd[0];
	return (0);
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
		
		// NOUVEAU: S'arrêter si on rencontre une redirection
		if (parser->current && (parser->current->type == T_REDIR_IN || 
			parser->current->type == T_REDIR_OUT || 
			parser->current->type == T_APPEND || 
			parser->current->type == T_HEREDOC))
			break;
		
		// S'arrêter si on rencontre un opérateur
		if (parser->current && get_precedence(parser->current->type) > PREC_NONE)
			break;
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

	// Parser la commande de base
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
	
	// NOUVEAU: Traiter toutes les redirections en séquence
	while (!at_end(parser) && (parser->current->type == T_REDIR_IN || 
			parser->current->type == T_REDIR_OUT || 
			parser->current->type == T_APPEND || 
			parser->current->type == T_HEREDOC))
	{
		if (parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND)
		{
			if (handle_output_redirection(parser, node) == -1)
				return (free_ast(node), NULL);
		}
		else if (parser->current->type == T_REDIR_IN)
		{
			if (handle_input_redirection(parser, node) == -1)
				return (free_ast(node), NULL);
		}
		else if (parser->current->type == T_HEREDOC)
		{
			if (handle_heredoc(parser, node) == -1)
				return (free_ast(node), NULL);
		}
	}
	
	return (node);
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
	return (result);
}
