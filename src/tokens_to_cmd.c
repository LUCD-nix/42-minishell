#include "../minishell.h"

// echo = 0 | cd == 1 | pwd == 2 | export == 3 | unset == 4 | env == 5 | exit == 6


int is_builtins(t_token *token)
{
	if (!token)
		return (NULL);

	if (token->type == T_WORD && (ft_strcmp(token->value, "cd") == 0))
		return (1);
}
