/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_utils2.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 13:23:18 by alvanaut          #+#    #+#             */
/*   Updated: 2025/10/06 18:30:00 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	collect_single_arg(t_parser *parser, char ***args, int *capacity,
		int count)
{
	if (!handle_capacity_resize(args, capacity, count))
		return (0);
	if (!add_current_arg(args, count, parser))
		return (0);
	advance(parser);
	return (1);
}

static int	process_quoted_section(char *str, int *i, char *result, int *j)
{
	char	quote;

	quote = str[*i];
	(*i)++;
	while (str[*i] && str[*i] != quote)
	{
		result[*j] = str[*i];
		(*j)++;
		(*i)++;
	}
	if (str[*i] == quote)
		(*i)++;
	return (0);
}

char	*remove_quotes_from_value(char *str)
{
	char	*result;
	int		i;
	int		j;

	if (!str)
		return (NULL);
	result = ft_calloc(ft_strlen(str) + 1, 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			process_quoted_section(str, &i, result, &j);
		else
			result[j++] = str[i++];
	}
	return (result[j] = '\0', result);
}

static int	should_continue_concat(t_parser *parser)
{
	if (!check(parser, T_WORD))
		return (0);
	if (!parser->current)
		return (0);
	if (should_continue_collection(parser))
		return (0);
	return (1);
}

char	*concat_arg_tokens(t_parser *parser)
{
	char	*result;
	char	*cleaned;

	result = ft_strdup(parser->current->value);
	if (!result)
		return (NULL);
	advance(parser);
	while (should_continue_concat(parser))
	{
		result = ft_strjoin_free_first(result, parser->current->value);
		if (!result)
			return (NULL);
		advance(parser);
	}
	cleaned = remove_quotes_from_value(result);
	free(result);
	return (cleaned);
}
