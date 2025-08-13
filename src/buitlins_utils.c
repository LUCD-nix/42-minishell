#include "../minishell.h"

// echo = 1 | cd == 2 | pwd == 3 | export == 4 | unset == 5| env == 6 | exit == 7

int is_builtins(t_token *token)
{
	if (!token && token->type != T_WORD)
		return (0);

	if (token->type == T_WORD && (ft_strncmp(token->value, "echo", 4) == 0))
		return (1);
	if (token->type == T_WORD && (ft_strncmp(token->value, "cd", 2) == 0))
		  return (2);
	if (token->type == T_WORD && (ft_strncmp(token->value, "pwd", 3) == 0))
		  return (3);
	if (token->type == T_WORD && (ft_strncmp(token->value, "export", 6) == 0))
		  return (4);
	if (token->type == T_WORD && (ft_strncmp(token->value, "unset", 5) == 0))
		  return (5);
	if (token->type == T_WORD && (ft_strncmp(token->value, "env", 3) == 0))
		  return (6);
	if (token->type == T_WORD && (ft_strncmp(token->value, "exit", 5) == 0))
		  return (7);
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

// Extraire les builtins jusqu au prochain builtins





// Creer t_command avec les builtins



