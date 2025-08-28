#include "../../minishell.h"

// char	*get_env_value(char *key, char **envp)
// {
// 	int		i;
// 	size_t	len;
//
// 	if (!key || !envp)
// 		return (NULL);
// 	len = ft_strlen(key);
// 	i = 0;
// 	while (envp[i])
// 	{
// 		if (ft_strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
// 			return (ft_strdup(envp[i] + len + 1));
// 		i++;
// 	}
// 	return (ft_strdup(""));
// }
//
static char	*expand_status(char *res, int *i, int last_status)
{
	char	*status;
	char	*new_res;

	status = ft_itoa(last_status);
	if (!status)
		return (res);
	new_res = ft_strjoin(res, status);
	free(status);
	free(res);
	(*i)++;
	return (new_res);
}

static char	*expand_env_var(char *res, char *value, int *i, t_list *env)
{
	int		start;
	char	*key;
	char	*val;
	char	*new_res;

	start = *i;
	while (ft_isalnum(value[*i]) || value[*i] == '_')
		(*i)++;
	key = ft_substr(value, start, *i - start);
	if (!key)
		return (res);
	val = env_get(env, key);
	if (!val)
		val = ft_strdup("");
	new_res = ft_strjoin(res, val);
	free(key);
	free(val);
	free(res);
	return (new_res);
}

static char	*append_char(char *res, char c)
{
	char	buf[2];
	char	*new_res;

	buf[0] = c;
	buf[1] = '\0';
	new_res = ft_strjoin(res, buf);
	free(res);
	return (new_res);
}

char	*expand_variables(char *value, t_list *envp, int last_status, t_quote_type quote)
{
	char	*res;
	int		i;

	if (!value)
		return (NULL);
	if (quote == Q_SIMPLE)
		return (ft_strdup(value));
	res = ft_strdup("");
	if (!res)
		return (NULL);
	i = 0;
	while (value[i])
	{
		if (value[i] == '$')
		{
			i++;
			if (value[i] == '?')
				res = expand_status(res, &i, last_status);
			else if (ft_isalnum(value[i]) || value[i] == '_')
				res = expand_env_var(res, value, &i, envp);
			else
				res = append_char(res, '$');
		}
		else
			res = append_char(res, value[i++]);
		if (!res)
			return (NULL);
	}
	return (res);
}

void	print_tokens(t_token *tokens)
{
	t_token	*tmp;

	tmp = tokens;
	while (tmp)
	{
		printf("value: [%s], type: %d, quote: %d\n", tmp->value, tmp->type, tmp->quote);
		tmp = tmp->next;
	}
}

