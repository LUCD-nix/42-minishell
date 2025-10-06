/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut < alvanaut@student.s19.be >       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 17:06:39 by lucorrei          #+#    #+#             */
/*   Updated: 2025/10/02 14:03:26 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termcap.h>
# include <termios.h>
# include <unistd.h>

# define PIPE_OUT 0
# define PIPE_IN 1
# define PIPE_LEFT 0
# define PIPE_RIGHT 1
# define CHILD_PID 0
# define PATH_MAX 4096

extern int			g_signal_received;

typedef int			t_file_desc;

typedef enum e_node_type
{
	NODE_CMD,
	NODE_BUILTIN,
	NODE_PIPE,
	NODE_AND,
	NODE_OR,
	NODE_REDIR_IN,
	NODE_REDIR_OUT,
	NODE_APPEND,
	NODE_HEREDOC,
	NODE_SUBSHELL
}					t_node_type;

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_AND,
	T_OR,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_APPEND,
	T_HEREDOC,
	T_LPAREN,
	T_RPAREN,
	T_EOF
}					t_token_type;

typedef enum e_quote_type
{
	Q_NONE,
	Q_SIMPLE,
	Q_DOUBLE
}					t_quote_type;

typedef enum e_precedence
{
	PREC_NONE,
	PREC_OR,
	PREC_AND,
	PREC_PIPE,
	PREC_REDIR,
	PREC_PRIMARY
}					t_precedence;

typedef struct s_lexeme
{
	char			*value;
	t_quote_type	quote;
}					t_lexeme;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	t_quote_type	quote;
	struct s_token	*next;
}					t_token;

typedef struct s_command
{
	char			*path;
	char			**args;
	int				argc;
}					t_command;

typedef struct s_redir
{
	t_node_type		type;
	char			*filename;
	int				fd;
	struct s_redir	*next;
}					t_redir;

typedef struct s_env
{
	char			*key;
	char			*value;
}					t_env;

typedef struct s_ast
{
	t_node_type		type;
	t_command		*command;
	char			*filename;
	int				fd_in;
	int				fd_out;
	struct s_ast	*left;
	struct s_ast	*right;
	t_list			**env;
	struct s_ast	*top;
	int				heredoc_quoted;
}					t_ast;

typedef struct s_parser
{
	t_token			*current;
	t_token			*tokens;
	int				had_error;
}					t_parser;

typedef struct s_expansion_data
{
	t_list			*envp;
	int				last_status;
}					t_expansion_data;

typedef struct s_heredoc_expand
{
	char			*line;
	char			*result;
	int				*i;
	int				*j;
	t_list			**env;
}					t_heredoc_expand;

typedef struct s_filename
{
	char			*value;
	int				has_quotes;
}					t_filename;

/* Lexer */
char				*read_line(void);
t_lexeme			create_separator_lexeme(char *line, int start, int len,
						int *i);
t_lexeme			handle_double_separators(char *line, int *i);
t_lexeme			handle_single_separators(char *line, int *i);
void				skip_spaces(char *line, int *i);
void				free_lexemes(t_lexeme *lex);
t_lexeme			handle_quote(char *line, int *i);
t_lexeme			handle_word(char *line, int *i);
t_lexeme			*lexer(char *line);
t_lexeme			*add_lexeme(t_lexeme *lexem, t_lexeme lex, int count);
t_lexeme			handle_separator(char *line, int *i);
int					is_separator(char *line);
int					is_quote_in_word(char *line, int pos);

/* Tokens */
t_token				*init_token(const char *line, t_token_type type,
						t_quote_type quote);
void				free_token(t_token *token);
t_token				*lexer_to_token(t_lexeme *lex);
t_token_type		get_token_type(char *token);

/* init_AST */
t_ast				*init_ast_node(t_node_type type, t_list **env);
t_ast				*init_cmd_node(t_command *cmd, t_list **env);
t_command			*init_cmd(char **args);
t_command			*allocate_command(void);

/* Free_AST */
void				free_redir(t_redir *redir);
void				free_cmd(t_command *cmd);
void				free_ast(t_ast *ast);

/*---Built-ins---*/
int					builtin_echo(int argc, char **argv);
int					builtin_cd(int argc, t_ast *node);
int					builtin_pwd(int argc);
int					builtin_export(int argc, t_ast *node);
int					builtin_unset(int argc, t_ast *node);
int					builtin_env(int argc, char **argv, char **envp);
int					builtin_exit(t_ast *node, int argc, char **argv,
						char **envp);
/* Envp*/
char				*expand_status(char *res, int *i, int last_status);
char				*expand_env_var(char *res, char *value, int *i,
						t_list *env);
char				*append_char(char *res, char c);
/*--Env--*/
t_list				*env_lst_add(t_list **lst, char *str);
t_list				*env_lst_from_str_arr(char **to_copy);
int					env_concat(char *str, t_list *env);
char				**env_lst_to_str_array(t_list *env);
char				*env_get(t_list *env, char *key);
int					env_set(t_list *env, char *key, char *value);
void				env_delete_key(t_list **head, char *key);
void				env_free(void *ptr);

/*---Pipes---*/
void				pipe_propagate_fd(t_ast *node);
void				pipe_right_routine(t_ast *node_pipe, int pipe_in,
						int pipe_out);
void				pipe_left_routine(t_ast *node_pipe, int pipe_in,
						int pipe_out);
int					pipe_wait_for_children(pid_t pids[2], int pipe_fd[2]);

/*---Traversal---*/
int					traverse_node(t_ast *node);
int					traverse_file(t_ast *node, int flags);
int					traverse_pipe(t_ast *node);
int					traverse_builtin(t_ast *node);
int					traverse_andor(t_ast *node, t_node_type type);
int					traverse_heredoc(t_ast *node);
void				andor_propagate_fd(t_ast *node);
int					traverse_redirect_builtin(t_ast *node, int *saved_in,
						int *saved_out);
int					handle_subshell_signal(int res);
void				exec_subshell_child(t_ast *node);
int					traverse_subshell(t_ast *node);
void				redir_propagate_fd(t_ast *node, int file_fd);
int					traverse_redir(t_ast *node);
int					check_delimiter_and_process(char *line, char *delimiter,
						int tmp_fd, t_ast *node);
/*---Execution---*/
int					exec_process(t_ast *command);
int					exec_builtin(t_ast *builtin);
int					exec_abs_path(t_ast *node);
int					path_not_found(void);
int					exec_get_path_loop(char **path_dirs, t_ast *node);

/* parse commands */
t_ast				*parse_command(t_parser *parser, t_list **env);
char				**resize_args_array(char **args, int *capacity);
char				**init_args_array(int *capacity);
int					handle_capacity_resize(char ***args, int *capacity,
						int count);
int					add_current_arg(char ***args, int count, t_parser *parser);
int					collect_single_arg(t_parser *parser, char ***args,
						int *capacity, int count);
int					should_continue_collection(t_parser *parser);

/* Parse main */
char				**collect_args(t_parser *parser, int *count);
t_ast				*parse_expression(t_parser *parser, t_precedence precedence,
						t_list **env);
void				propagate_first_node(t_ast *first, t_ast *current);
t_ast				*parse(t_token *tokens, t_list **env);
int					is_redirection_operator(t_token_type op_type);
int					is_binary_operator(t_token_type op_type);
t_ast				*handle_redirection_operator(t_parser *parser, t_ast *left,
						t_list **env);

/* Parse operations */
t_ast				*parse_binary(t_parser *parser, t_ast *left,
						t_token_type op, t_list **env);
t_ast				*parse_primary(t_parser *parser, t_list **env);

/* Parse redirections */
t_ast				*parse_single_redirection(t_parser *parser, t_list **env);
t_ast				*reorganize_redirections(t_ast *node);
t_ast				*collect_all_redirections(t_parser *parser, t_ast *cmd,
						t_list **env, t_ast *top_redir);
t_ast				*parse_command_with_redirections(t_parser *parser,
						t_list **env);
t_ast				*handle_binary_operator(t_parser *parser, t_ast *left,
						t_token_type op_type, t_list **env);
t_ast				*process_operator(t_parser *parser, t_ast *left,
						t_token_type op_type, t_list **env);
t_ast				*create_redirection_node(t_token_type redir_type,
						t_list **env);
t_ast				*parse_single_redirection(t_parser *parser, t_list **env);
t_ast				*reorganize_redirections(t_ast *node);
int					is_redirection_token(t_parser *parser);

/* Pratt Parser */
t_ast				*parse(t_token *tokens, t_list **env);
t_precedence		get_precedence(t_token_type type);
int					is_builtin(char *value);

/* Parser utils */
void				advance(t_parser *parser);
int					match(t_parser *parser, t_token_type type);
int					check(t_parser *parser, t_token_type type);
void				error(t_parser *parser, char *message);
int					at_end(t_parser *parser);

/* Environment */
char				*get_env_value(char *key, t_list *envp);
char				*expand_variables(char *value, t_list *envp,
						int last_status, t_quote_type quote);

/* Utils */
void				ft_free_tab(char **tab);
void				*ft_realloc(void *ptr, size_t size);
void				exit_and_free(t_ast *node, int exit_code, char *message);
void				close_fds(t_ast *node);

/* Signals*/
void				handle_sigint_interactive(int sig);
void				handle_sigint_child(int sig);
void				handle_sigquit_child(int sig);
void				setup_interactive_signals(void);
void				setup_child_signals(void);
void				setup_heredoc_signals(void);
int					ignore_signals(void);
int					check_signal_status(void);
void				handle_sigint_heredoc(int sig);

/* Traversal Heredoc */
t_file_desc			open_tmp_file(t_file_desc fd);
char				*clean_line_copy(char *line);
int					is_delimiter_line(char *line, char *delimiter);
int					should_expand_variables(t_ast *node);
char				*get_clean_delimiter(char *filename);
int					contains_variables(char *line);
int					get_var_name_len(char *line, int start);
char				*extract_var_name(char *line, int start, int len);
char				*get_env_var_value(char *var_name, t_list **env);
void				copy_var_value(char *result, int *j, char *var_value);
char				*process_heredoc_line(char *line, t_ast *node);
void				process_character_bis(t_heredoc_expand *data);
char				*expand_line_variables(char *line, t_list **env);
void				expand_variable(t_heredoc_expand *data);
int					read_and_process_input(int tmp_file_write, t_ast *node);
int					traverse_heredoc(t_ast *node);
int					handle_heredoc_eof(char *line, char *delimiter);
int					handle_signal_interruption(char *line, char *delimiter);
int					process_heredoc_line_input(char *line, char *delimiter,
						int tmp_fd, t_ast *node);
int					handle_heredoc_eof(char *line, char *delimiter);
int					handle_signal_interruption(char *line, char *delimiter);
int					process_heredoc_line_input(char *line, char *delimiter,
						int tmp_fd, t_ast *node);
/* Main */
void				handle_interactive_signals(void);
void				expand_tokens(t_token *tokens, t_list *envp,
						int last_status);
void				init_minimal_env(t_list **my_envp);
int					init_environment(char **envp, t_list **my_envp);
int					handle_empty_line(char *line);
int					handle_exit_command(char *line);
int					process_parsed_line(t_token *tokens, t_list **envp,
						int *last_status);
int					process_line(char *line, t_list **envp, int *last_status);
void				print_welcome_message(void);

#endif
