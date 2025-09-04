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
	node->right = parse_expression_bash_compatible(parser, get_precedence(op) + 1, env);
	if (!node->right)
		return (free_ast(node), NULL);
	return (node);
}

t_ast	*parse_expression_bash_compatible(t_parser *parser, t_precedence precedence,
						t_list **env)
{
	t_ast			*left;
	t_token_type	op_type;

	left = parse_primary_bash_compatible(parser, env);
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

t_ast	*parse_primary_bash_compatible(t_parser *parser, t_list **env)
{
	if (check(parser, T_LPAREN))
		return (parse_subshell_bash_compatible(parser, env));
	if (check(parser, T_WORD))
		return (parse_command_bash_compatible(parser, env));
	error(parser, "Expected command or '('");
	return (NULL);
}

static int	handle_output_redirection_bash_compatible(t_parser *parser, t_ast *cmd)
{
	t_token_type	redir_type;
	int				temp_fd;
	int				flags;

	redir_type = parser->current->type;
	advance(parser);
	
	if (!check(parser, T_WORD))
	{
		error(parser, "Expected filename after redirection");
		return (1);  // FIX: retourner 1 au lieu de -1
	}
	
	flags = O_WRONLY | O_CREAT | ((redir_type == T_APPEND) ? O_APPEND : O_TRUNC);
	temp_fd = open(parser->current->value, flags, 0644);
	if (temp_fd == -1)
	{
		perror("minishell: output redirection");
		return (1);  // FIX: retourner 1 au lieu de -1
	}
	close(temp_fd);
	
	if (cmd->output_file)
	{
		free(cmd->output_file);
		cmd->output_file = NULL;
	}
	
	cmd->output_file = ft_strdup(parser->current->value);
	if (!cmd->output_file)
	{
		error(parser, "Memory allocation failed");
		return (1);  // FIX: retourner 1 au lieu de -1
	}
	
	cmd->append_mode = (redir_type == T_APPEND) ? 1 : 0;
	advance(parser);
	return (0);
}

static int	handle_input_redirection_bash_compatible(t_parser *parser, t_ast *cmd)
{
	int	temp_fd;

	advance(parser);
	
	if (!check(parser, T_WORD))
	{
		error(parser, "Expected filename after redirection");
		return (1);
	}
	
	// NOUVEAU: Vérifier que le fichier existe et est lisible
	temp_fd = open(parser->current->value, O_RDONLY);
	if (temp_fd == -1)
	{
		perror("minishell: input redirection");
		return (1);
	}
	close(temp_fd); // Fermer immédiatement
	
	// Libérer l'ancien fichier d'entrée s'il existe
	if (cmd->input_file)
	{
		free(cmd->input_file);
		cmd->input_file = NULL;
	}
	
	// Assigner le nouveau fichier d'entrée (seul le dernier sera utilisé)
	cmd->input_file = ft_strdup(parser->current->value);
	if (!cmd->input_file)
	{
		error(parser, "Memory allocation failed");
		return (1);
	}
	
	advance(parser);
	return (0);
}

static int	handle_heredoc_new(t_parser *parser, t_ast *cmd)
{
	char	*delimiter;
	char	*line;
	char	*expanded_line;
	int		pipe_fd[2];
	size_t	delim_len;
	int		should_expand;

	advance(parser);
	
	if (!check(parser, T_WORD))
	{
		error(parser, "Expected delimiter after <<");
		return (1);
	}
	
	delimiter = parser->current->value;
	delim_len = ft_strlen(delimiter);
	
	// NOUVEAU: Déterminer si on doit expand selon les guillemets du délimiteur
	should_expand = 1;  // Par défaut, on expand
	if (parser->current->quote == Q_SIMPLE || parser->current->quote == Q_DOUBLE)
		should_expand = 0;  // Pas d'expansion si délimiteur entre guillemets
	
	advance(parser);
	
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	
	while (1)
	{
		line = readline("heredoc> ");
		if (!line)  // Ctrl+D
		{
			printf("\n");
			break;
		}
		
		// Vérifier si on a atteint le délimiteur
		if (ft_strlen(line) == delim_len && 
			ft_strncmp(line, delimiter, delim_len) == 0)
		{
			free(line);
			break;
		}
		
		// NOUVEAU: Expand les variables si nécessaire
		if (should_expand)
		{
			expanded_line = expand_variables(line, *cmd->env, 0, Q_NONE);
			if (expanded_line)
			{
				write(pipe_fd[1], expanded_line, ft_strlen(expanded_line));
				free(expanded_line);
			}
			else
			{
				write(pipe_fd[1], line, ft_strlen(line));
			}
		}
		else
		{
			write(pipe_fd[1], line, ft_strlen(line));
		}
		
		write(pipe_fd[1], "\n", 1);
		free(line);
	}
	
	close(pipe_fd[1]);
	
	if (cmd->heredoc_fd >= 0)
		close(cmd->heredoc_fd);
	
	cmd->heredoc_fd = pipe_fd[0];
	return (0);
}

static int	apply_redirections_to_subshell_bash_compatible(t_parser *parser, t_ast *subshell)
{
	while (!at_end(parser) && (parser->current->type == T_REDIR_IN || 
			parser->current->type == T_REDIR_OUT || 
			parser->current->type == T_APPEND || 
			parser->current->type == T_HEREDOC))
	{
		if (parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND)
		{
			if (handle_output_redirection_bash_compatible(parser, subshell) == -1)
				return (-1);
		}
		else if (parser->current->type == T_REDIR_IN)
		{
			if (handle_input_redirection_bash_compatible(parser, subshell) == -1)
				return (-1);
		}
		else if (parser->current->type == T_HEREDOC)
		{
			if (handle_heredoc_new(parser, subshell) == -1)
				return (-1);
		}
	}
	return (0);
}

t_ast	*parse_subshell_bash_compatible(t_parser *parser, t_list **env)
{
	t_ast	*node;

	advance(parser); // Passer '('
	
	node = init_ast_node(NODE_SUBSHELL, env);
	if (!node)
		return (error(parser, "Memory allocation failed"), NULL);
	
	node->left = parse_expression_bash_compatible(parser, PREC_NONE, env);
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
	if (apply_redirections_to_subshell_bash_compatible(parser, node) == -1)
		return (free_ast(node), NULL);
	
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
		// NOUVEAU: Ignorer les tokens vides après expansion
		if (!parser->current->value || ft_strlen(parser->current->value) == 0)
		{
			advance(parser);
			continue;
		}

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
		
		if (parser->current && (parser->current->type == T_REDIR_IN || 
			parser->current->type == T_REDIR_OUT || 
			parser->current->type == T_APPEND || 
			parser->current->type == T_HEREDOC))
			break;
		
		if (parser->current && get_precedence(parser->current->type) > PREC_NONE)
			break;
	}
	
	// NOUVEAU: Si aucun argument valide, retourner NULL
	if (*count == 0)
	{
		free(args);
		return (NULL);
	}
	
	args[*count] = NULL;
	return (args);
}

t_ast	*parse_command_bash_compatible(t_parser *parser, t_list **env)
{
	char		**args;
	int			argc;
	t_command	*cmd;
	t_ast		*node;
	int			redir_error;

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
	
	// FIX: Traiter les redirections et propager les erreurs correctement
	while (!at_end(parser) && (parser->current->type == T_REDIR_IN || 
			parser->current->type == T_REDIR_OUT || 
			parser->current->type == T_APPEND || 
			parser->current->type == T_HEREDOC))
	{
		redir_error = 0;
		if (parser->current->type == T_REDIR_OUT || parser->current->type == T_APPEND)
		{
			redir_error = handle_output_redirection_bash_compatible(parser, node);
		}
		else if (parser->current->type == T_REDIR_IN)
		{
			redir_error = handle_input_redirection_bash_compatible(parser, node);
		}
		else if (parser->current->type == T_HEREDOC)
		{
			redir_error = handle_heredoc_new(parser, node);
		}
		
		// FIX: Si erreur de redirection, marquer le nœud comme ayant une erreur
		if (redir_error != 0)
		{
			// Marquer l'erreur dans le nœud pour la gestion ultérieure
			node->fd_in = -2;  // Utiliser -2 comme marqueur d'erreur
			break;
		}
	}
	
	return (node);
}

t_ast	*parse_new(t_token *tokens, t_list **env)
{
	t_parser	parser;
	t_ast		*result;

	parser.tokens = tokens;
	parser.current = tokens;
	parser.had_error = 0;
	if (at_end(&parser))
		return (NULL);
	result = parse_expression_bash_compatible(&parser, PREC_NONE, env);
	if (parser.had_error || !at_end(&parser))
	{
		if (result)
			free_ast(result);
		return (NULL);
	}
	return (result);
}
