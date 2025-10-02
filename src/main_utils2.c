/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 16:58:09 by alvanaut          #+#    #+#             */
/*   Updated: 2025/09/28 16:58:11 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_exit_command(char *line)
{
	if (ft_strcmp(line, "exit") == 0)
		return (0);
	return (-1);
}

int	process_parsed_line(t_token *tokens, t_list **envp,
		int *last_status)
{
	t_ast	*ast;
	int		result;

	expand_tokens(tokens, *envp, *last_status);
	ast = parse(tokens, envp);
	free_token(tokens);
	if (!ast)
	{
		perror("Parse error: Invalid syntax\n");
		*last_status = 2;
		return (1);
	}
	result = traverse_node(ast);
	free_ast(ast);
	if (result == -1)
	{
		*last_status = 130;
		return (1);
	}
	*last_status = result;
	return (1);
}

int	process_line(char *line, t_list **envp, int *last_status)
{
	t_lexeme	*lexemes;
	t_token		*tokens;
	int			empty_check;
	int			exit_check;

	empty_check = handle_empty_line(line);
	if (empty_check != -1)
		return (empty_check);
	exit_check = handle_exit_command(line);
	if (exit_check != -1)
		return (exit_check);
	lexemes = lexer(line);
	if (!lexemes)
		return (1);
	tokens = lexer_to_token(lexemes);
	free_lexemes(lexemes);
	if (!tokens)
		return (1);
	return (process_parsed_line(tokens, envp, last_status));
}

void	print_welcome_message(void)
{
	printf("Minishell - Parsing Test Version\n");
	printf("Type 'exit' to quit\n\n");
}
