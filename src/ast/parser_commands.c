#include "../../minishell.h"

#include "../../minishell.h"

static char	**resize_args_array(char **args, int *capacity)
{
	char	**new_args;
	int		i;

	*capacity *= 2;
	new_args = malloc(sizeof(char *) * (*capacity));
	if (!new_args)
		return (NULL);
	i = 0;
	while (args && args[i])
	{
		new_args[i] = args[i];
		i++;
	}
	free(args);
	return (new_args);
}

static char	**init_args_array(int *capacity)
{
	char	**args;

	*capacity = 4;
	args = malloc(sizeof(char *) * (*capacity));
	return (args);
}

static int	handle_capacity_resize(char ***args, int *capacity, int count)
{
	if (count >= *capacity - 1)
	{
		*args = resize_args_array(*args, capacity);
		if (!*args)
			return (0);
	}
	return (1);
}

static int	add_current_arg(char ***args, int count, t_parser *parser)
{
	(*args)[count] = ft_strdup(parser->current->value);
	if (!(*args)[count])
	{
		ft_free_tab(*args);
		return (0);
	}
	return (1);
}

static int	should_continue_collection(t_parser *parser)
{
	if (!parser->current)
		return (0);
	if (get_precedence(parser->current->type) > PREC_NONE)
		return (0);
	return (1);
}

char	**collect_args(t_parser *parser, int *count)
{
	char	**args;
	int		capacity;

	*count = 0;
	args = init_args_array(&capacity);
	if (!args)
		return (NULL);
	while (check(parser, T_WORD))
	{
		if (!handle_capacity_resize(&args, &capacity, *count))
			return (NULL);
		if (!add_current_arg(&args, *count, parser))
			return (NULL);
		(*count)++;
		advance(parser);
		if (!should_continue_collection(parser))
			break ;
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

	args = collect_args(parser, &argc);
	if (!args || argc == 0)
	{
		error(parser, "Expected command");
		return (NULL);
	}
	cmd = init_cmd(args);
	ft_free_tab(args);
	if (!cmd)
	{
		error(parser, "Memory allocation failed");
		return (NULL);
	}
	node = init_cmd_node(cmd, env);
	if (!node)
	{
		free_cmd(cmd);
		error(parser, "Memory allocation failed");
		return (NULL);
	}
	return (node);
}
