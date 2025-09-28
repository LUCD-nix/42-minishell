#include "../minishell.h"

void	handle_interactive_signals(void)
{
	if (g_signal_received == SIGINT)
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_signal_received = 0;
	}
}

void	expand_tokens(t_token *tokens, t_list *envp, int last_status)
{
	t_token	*tmp;
	char	*expanded;

	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == T_WORD)
		{
			expanded = expand_variables(tmp->value, envp, last_status,
					tmp->quote);
			if (expanded)
			{
				free(tmp->value);
				tmp->value = expanded;
			}
		}
		tmp = tmp->next;
	}
}

void	init_minimal_env(t_list **my_envp)
{
	char	*pwd_str;
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		pwd_str = ft_strjoin("PWD=", cwd);
		if (pwd_str)
		{
			env_lst_add(my_envp, pwd_str);
			free(pwd_str);
		}
		free(cwd);
	}
	env_lst_add(my_envp, "SHLVL=1");
	env_lst_add(my_envp, "_=/usr/bin/env");
}

int	init_environment(char **envp, t_list **my_envp)
{
	if (!envp || !envp[0])
	{
		*my_envp = NULL;
		init_minimal_env(my_envp);
	}
	else
	{
		*my_envp = env_lst_from_str_arr(envp);
	}
	if (!*my_envp)
	{
		perror("Error: Failed to copy environment\n");
		return (1);
	}
	return (0);
}

int	handle_empty_line(char *line)
{
	if (!line)
		return (0);
	if (ft_strlen(line) == 0)
		return (1);
	return (-1);
}
