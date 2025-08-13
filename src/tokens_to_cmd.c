#include "../minishell.h"

// echo == 1 | cd == 2 | pwd == 3 | export == 4 | unset == 5| env == 6 | exit == 7

int is_builtins(t_token *token)
{
	if (!token || token->type != T_WORD)
		return (0);

	if (ft_strncmp(token->value, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(token->value, "cd", 3) == 0)
		  return (2);
	if (ft_strncmp(token->value, "pwd", 4) == 0)
		  return (3);
	if (ft_strncmp(token->value, "export", 7) == 0)
		  return (4);
	if (ft_strncmp(token->value, "unset", 6) == 0)
		  return (5);
	if (ft_strncmp(token->value, "env", 4) == 0)
		  return (6);
	if (ft_strncmp(token->value, "exit", 6) == 0)
		  return (7);
	return (-1);
}

// prendre le path

char *builtins_path(int builtins)
{
	if (builtins == 0)
		return (NULL);
	if (builtins == 1)
		return (ft_strdup("echo"));
	if (builtins == 2)
		return (ft_strdup("cd"));
	if (builtins == 3)
		return (ft_strdup("pwd"));
	if (builtins == 4)
		return (ft_strdup("export"));
	if (builtins == 5)
		return (ft_strdup("unset"));
	if (builtins == 6)
		return (ft_strdup("env"));
	if (builtins == 7)
		return (ft_strdup("exit"));
	return (NULL);
}

// Trouver la taille entre deux builtins

int		builtins_size(t_token *token)
{
	int size;

	if (!token)
		return (0);

	size = 0;
	token = token->next;
	while (token && is_builtins(token) == -1)
	{
		size++;
		token = token->next;
	}
	return (size);
}
// Trouver les builtins puis extraire jusqu au prochain


//char **builtins_args(t_token *token)
//{
//	int size;
//	char **args;
//	int i;
//
//	i = 0;
//	size = builtins_size(token) + 1;
//	args = malloc(sizeof(char *) * (size + 1));
//	if (!args)
//		return (NULL);
//	args[i++] = ft_strdup(token->value);
//	token = token->next;
//	while (token && i < size)
//	{
//		if (is_builtins(token))
//			break ;
//		args[i] = ft_strdup(token->value);
//		if (!args[i])
//			return (free_args(args), NULL);
//		i++;
//		token = token->next;
//	}
//	args[i] = NULL;
//	return (args);
//}

int	fill_args_from_tokens(t_token *token, char **args, int start)
{
	int	i;

	i = start;
	while (token && is_builtins(token) == -1)
	{
		args[i] = ft_strdup(token->value);
		if (!args[i])
			return (-1);
		i++;
		token = token->next;
	}
	args[i] = NULL;
	return (0);
}

t_command	*token_to_builtin_cmd(t_token *token)
{
	int		id;
	int		size;
	char	*path;
	char	**args;

	if (!token || (id = is_builtins(token)) <= 0)
		return (NULL);
	path = builtins_path(id);
	if (!path)
		return (NULL);
	size = builtins_size(token);
	args = malloc(sizeof(char *) * (size + 2));
	if (!args || !(args[0] = ft_strdup(path)))
		return (free(path), free(args), NULL);
	if (fill_args_from_tokens(token->next, args, 1) == -1)
		return (free_args(args), free(path), NULL);
	return (init_command(path, args));
}

