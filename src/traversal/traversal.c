/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traversal.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucorrei <lucorrei@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:25:38 by lucorrei          #+#    #+#             */
/*   Updated: 2025/09/03 16:25:40 by lucorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../minishell.h"

void	andor_propagate_fd(t_ast *node)
{
	node->left->fd_in = node->fd_in;
	node->left->fd_out = node->fd_out;
	node->right->fd_in = node->fd_in;
	node->right->fd_out = node->fd_out;
}

static int	count_redirections(t_ast *node)
{
	int	count = 0;
	t_ast *current = node;

	while (current && (current->type == NODE_REDIR_IN || 
			current->type == NODE_REDIR_OUT || 
			current->type == NODE_APPEND || 
			current->type == NODE_HEREDOC))
	{
		count++;
		current = current->left;
	}
	return (count);
}

int	create_heredoc_fd(t_ast *node)
{
	int		tmp_file;
	char	*line;
	size_t	delim_len;
	char	*temp_filename;
	char	*pid_str;
	char	*counter_str;
	static int heredoc_counter = 0;

	// Créer un nom de fichier unique pour éviter les conflits
	pid_str = ft_itoa(getpid());
	counter_str = ft_itoa(heredoc_counter++);
	if (!pid_str || !counter_str)
	{
		free(pid_str);
		free(counter_str);
		return (-1);
	}
	
	temp_filename = ft_strjoin("/tmp/minishell_heredoc_", pid_str);
	if (temp_filename)
	{
		char *temp = ft_strjoin(temp_filename, "_");
		free(temp_filename);
		temp_filename = temp;
		if (temp_filename)
		{
			temp = ft_strjoin(temp_filename, counter_str);
			free(temp_filename);
			temp_filename = temp;
		}
	}
	
	free(pid_str);
	free(counter_str);
	
	if (!temp_filename)
		return (-1);

	delim_len = ft_strlen(node->filename);
	tmp_file = open(temp_filename, O_CREAT | O_TRUNC | O_RDWR, 0600);
	if (tmp_file == -1)
	{
		perror("minishell: heredoc tmp file");
		free(temp_filename);
		return (-1);
	}
	
	printf("heredoc> ");
	line = get_next_line(STDIN_FILENO);
	while (line)
	{
		// Vérifier si on a atteint le délimiteur
		if (ft_strlen(line) == delim_len + 1 && 
			ft_strncmp(line, node->filename, delim_len) == 0 && 
			line[delim_len] == '\n')
		{
			free(line);
			break;
		}
		
		write(tmp_file, line, ft_strlen(line));
		free(line);
		printf("heredoc> ");
		line = get_next_line(STDIN_FILENO);
	}
	
	// Repositionner au début du fichier pour la lecture
	if (lseek(tmp_file, 0, SEEK_SET) == -1)
	{
		perror("minishell: lseek heredoc");
		close(tmp_file);
		unlink(temp_filename);
		free(temp_filename);
		return (-1);
	}
	
	// Note: le fichier sera supprimé quand le fd sera fermé
	unlink(temp_filename);
	free(temp_filename);
	return (tmp_file);
}

static int	setup_input_redirection(t_ast *node)
{
	int	fd;

	if (node->type == NODE_HEREDOC)
	{
		// Gérer heredoc directement
		return (create_heredoc_fd(node));
	}
	else if (node->type == NODE_REDIR_IN)
	{
		fd = open(node->filename, O_RDONLY);
		if (fd == -1)
		{
			perror("minishell: input redirection");
			return (-1);
		}
		return (fd);
	}
	return (-2); // Pas une redirection d'entrée
}

static int	setup_output_redirection(t_ast *node)
{
	int	fd;
	int	flags;

	if (node->type == NODE_REDIR_OUT)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (node->type == NODE_APPEND)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		return (-2); // Pas une redirection de sortie

	fd = open(node->filename, flags, 0644);
	if (fd == -1)
	{
		perror("minishell: output redirection");
		return (-1);
	}
	return (fd);
}

int	traverse_multiple_redirections(t_ast *node)
{
	int		input_fd = -2;
	int		output_fd = -2;
	t_ast	*current = node;
	int		res;
	int		temp_fd;

	while (current && (current->type == NODE_REDIR_IN || 
			current->type == NODE_REDIR_OUT || 
			current->type == NODE_APPEND || 
			current->type == NODE_HEREDOC))
	{
		if (current->type == NODE_REDIR_IN || current->type == NODE_HEREDOC)
		{
			if (input_fd >= 0)
				close(input_fd);
			temp_fd = setup_input_redirection(current);
			if (temp_fd == -1)
			{
				if (output_fd >= 0)
					close(output_fd);
				return (1); // FIX: retourner 1 au lieu de -1 pour les erreurs de redirection
			}
			if (temp_fd >= 0)
				input_fd = temp_fd;
		}
		else
		{
			if (output_fd >= 0)
				close(output_fd);
			temp_fd = setup_output_redirection(current);
			if (temp_fd == -1)
			{
				if (input_fd >= 0)
					close(input_fd);
				return (1); // FIX: retourner 1 au lieu de -1
			}
			if (temp_fd >= 0)
				output_fd = temp_fd;
		}
		current = current->left;
	}

	if (!current)
	{
		if (input_fd >= 0)
			close(input_fd);
		if (output_fd >= 0)
			close(output_fd);
		return (1);
	}

	current->fd_in = (input_fd >= 0) ? input_fd : node->fd_in;
	current->fd_out = (output_fd >= 0) ? output_fd : node->fd_out;

	res = traverse_node_new(current);

	if (input_fd >= 0)
		close(input_fd);
	if (output_fd >= 0)
		close(output_fd);

	return (res);
}

int	traverse_redir(t_ast *node)
{
	int	redir_count;

	// Compter les redirections imbriquées
	redir_count = count_redirections(node);
	
	if (redir_count > 1)
		return (traverse_multiple_redirections(node));
	
	// Redirection simple - méthode originale optimisée
	int	file_fd;
	int	o_flags;
	int	res;

	if (node->type == NODE_REDIR_IN)
		o_flags = O_RDONLY;
	else if (node->type == NODE_REDIR_OUT)
		o_flags = O_WRONLY | O_CREAT | O_TRUNC;
	else // NODE_APPEND
		o_flags = O_WRONLY | O_CREAT | O_APPEND;
	
	file_fd = open(node->filename, o_flags, 0644);
	if (file_fd == -1)
	{
		perror("minishell: redirection");
		return (-1);
	}

	// Propager les fd du parent
	node->left->fd_in = node->fd_in;
	node->left->fd_out = node->fd_out;
	
	// Assigner le nouveau fd
	if (node->type == NODE_REDIR_IN)
		node->left->fd_in = file_fd;
	else
		node->left->fd_out = file_fd;
	
	res = traverse_node_new(node->left);
	close(file_fd);
	return (res);
}

int	traverse_heredoc(t_ast *node)
{
	int		heredoc_fd;
	int		res;

	heredoc_fd = create_heredoc_fd(node);
	if (heredoc_fd == -1)
		return (-1);
	
	// Propager les fd et rediriger l'entrée
	node->left->fd_in = heredoc_fd;
	node->left->fd_out = node->fd_out;
	
	res = traverse_node_new(node->left);
	close(heredoc_fd);
	return (res);
}

int	traverse_subshell_new(t_ast *node)
{
	pid_t	pid;
	int		status;
	int		input_fd;
	int		output_fd;

	// Préparer les fd pour le subshell
	input_fd = node->fd_in;
	output_fd = node->fd_out;
	
	// Gérer les redirections du subshell
	if (node->input_file)
	{
		input_fd = open(node->input_file, O_RDONLY);
		if (input_fd == -1)
		{
			perror("minishell: input redirection");
			return (1);
		}
	}
	else if (node->heredoc_fd >= 0)
	{
		input_fd = node->heredoc_fd;
	}
	
	if (node->output_file)
	{
		int flags = O_WRONLY | O_CREAT | (node->append_mode ? O_APPEND : O_TRUNC);
		output_fd = open(node->output_file, flags, 0644);
		if (output_fd == -1)
		{
			if (node->input_file && input_fd >= 0)
				close(input_fd);
			perror("minishell: output redirection");
			return (1);
		}
	}

	pid = fork();
	if (pid == -1)
		return (perror("minishell: fork"), -1);
	
	if (pid == 0)
	{
		// Dans le processus enfant
		if (input_fd != STDIN_FILENO && dup2(input_fd, STDIN_FILENO) == -1)
			exit(1);
		if (output_fd != STDOUT_FILENO && dup2(output_fd, STDOUT_FILENO) == -1)
			exit(1);
		
		exit(traverse_node_new(node->left));
	}
	
	// Nettoyer les fd dans le parent
	if (node->input_file && input_fd >= 0)
		close(input_fd);
	if (node->output_file && output_fd >= 0)
		close(output_fd);
	
	// Attendre le processus enfant
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (-1);
}

int	traverse_command_simple(t_ast *node)
{
	int	input_fd;
	int	output_fd;
	int	res;

	input_fd = node->fd_in;
	output_fd = node->fd_out;
	
	// Gérer le fichier d'entrée
	if (node->input_file)
	{
		input_fd = open(node->input_file, O_RDONLY);
		if (input_fd == -1)
		{
			perror("minishell: input redirection");
			return (1);
		}
	}
	else if (node->heredoc_fd >= 0)
	{
		input_fd = node->heredoc_fd;
	}
	
	// Gérer le fichier de sortie
	if (node->output_file)
	{
		int flags = O_WRONLY | O_CREAT | (node->append_mode ? O_APPEND : O_TRUNC);
		output_fd = open(node->output_file, flags, 0644);
		if (output_fd == -1)
		{
			if (node->input_file && input_fd >= 0)
				close(input_fd);
			perror("minishell: output redirection");
			return (1);
		}
	}
	
	// Assigner les fd à la commande
	node->fd_in = input_fd;
	node->fd_out = output_fd;
	
	// Exécuter la commande
	if (node->type == NODE_BUILTIN)
		res = traverse_builtin(node);
	else
		res = exec_process(node);
	
	// Nettoyer les fd
	if (node->input_file && input_fd >= 0)
		close(input_fd);
	if (node->output_file && output_fd >= 0)
		close(output_fd);
	
	return (res);
}

int	traverse_node_new(t_ast *node)
{
	int			res;
	t_node_type	type;

	if (!node)
		return (-1);
		
	res = -1;
	type = node->type;
	
	if (type == NODE_CMD || type == NODE_BUILTIN)
	{
		// NOUVEAU: Utiliser la logique simplifiée si pas de redirections complexes
		if (!node->left && !node->right)
			res = traverse_command_simple(node);
		else if (type == NODE_CMD)
			res = exec_process(node);
		else
			res = traverse_builtin(node);
	}
	else if (type == NODE_PIPE)
		res = traverse_pipe(node);
	else if (type == NODE_REDIR_IN || type == NODE_REDIR_OUT || type == NODE_APPEND)
		res = traverse_redir(node);  // Pour compatibilité avec ancien code
	else if (type == NODE_AND || type == NODE_OR)
		res = traverse_andor(node, type);
	else if (type == NODE_HEREDOC)
		res = traverse_heredoc(node);  // Pour compatibilité
	else if (type == NODE_SUBSHELL)
		res = traverse_subshell_new(node);  // NOUVEAU
		
	return (res);
}
