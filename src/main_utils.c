/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 16:57:57 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 16:57:59 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_interactive_signals(void)
{
	if (g_signal_received == SIGINT)
	{
		g_signal_received = 0;
	}
}

void	expand_tokens(t_token *tokens, t_list *envp, int last_status)
{
	t_token	*tmp;
	t_token	*prev;
	char	*expanded;

	tmp = tokens;
	prev = NULL;
	while (tmp)
	{
		if (tmp->type == T_WORD && (!prev || prev->type != T_HEREDOC))
		{
			expanded = expand_variables(tmp->value, envp, last_status,
					tmp->quote);
			if (expanded)
			{
				free(tmp->value);
				tmp->value = expanded;
			}
		}
		prev = tmp;
		tmp = tmp->next;
	}
}

static void	increment_shlvl(t_list **my_envp)
{
	char	*current_shlvl;
	char	*new_shlvl_str;
	char	*shlvl_value;
	int		shlvl;

	current_shlvl = env_get(*my_envp, "SHLVL");
	if (current_shlvl)
	{
		shlvl = ft_atoi(current_shlvl);
		shlvl++;
	}
	else
		shlvl = 1;
	shlvl_value = ft_itoa(shlvl);
	if (!shlvl_value)
		return ;
	new_shlvl_str = ft_strjoin("SHLVL=", shlvl_value);
	free(shlvl_value);
	if (!new_shlvl_str)
		return ;
	env_delete_key(my_envp, "SHLVL");
	env_lst_add(my_envp, new_shlvl_str);
	free(new_shlvl_str);
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
		if (!*my_envp)
		{
			perror("Error: Failed to copy environment\n");
			return (1);
		}
		increment_shlvl(my_envp);
	}
	return (0);
}

