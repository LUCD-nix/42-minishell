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
# define CHILD_PID 0
# define PATH_MAX 4096

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
}	t_node_type;

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
}	t_token_type;

typedef enum e_quote_type
{
	Q_NONE,
	Q_SIMPLE,
	Q_DOUBLE
}	t_quote_type;

typedef enum e_precedence
{
	PREC_NONE,
	PREC_OR,
	PREC_AND,
	PREC_PIPE,
	PREC_REDIR,
	PREC_PRIMARY
}	t_precedence;

typedef struct s_lexeme
{
	char			*value;
	t_quote_type	quote;
}	t_lexeme;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	t_quote_type	quote;
	struct s_token	*next;
}	t_token;

typedef struct s_command
{
	char	*path;
	char	**args;
	int		argc;
}	t_command;

typedef struct s_redir
{
<<<<<<< HEAD
	char	*key;
	char	*value;
}	t_env;
=======
	t_node_type		type;
	char			*filename;
	int				fd;
	struct s_redir	*next;
}	t_redir;
>>>>>>> master

typedef struct s_ast
{
	t_node_type		type;
	t_command		*command;
	char			*filename;
	int				fd_in;
	int				fd_out;
	struct s_ast	*left;
	struct s_ast	*right;
	t_list 			**env;
}	t_ast;

typedef struct s_parser
{
	t_token	*current;
	t_token	*tokens;
	int		had_error;
}	t_parser;

/* Lexer */
t_lexeme		*lexer(char *line);
void			free_lexemes(t_lexeme *lex);
char			*read_line(void);

/* Tokens */
t_token			*init_token(const char *line, t_token_type type, t_quote_type quote);
void			free_token(t_token *token);
t_token			*lexer_to_token(t_lexeme *lex);
t_token_type	get_token_type(char *token);

/* AST */
t_ast			*init_ast_node(t_node_type type);
t_ast			*init_cmd_node(t_command *cmd);
void			free_ast(t_ast *ast);
void			free_cmd(t_command *cmd);
t_command		*init_cmd(char **args);

/*---Built-ins---*/
int					builtin_echo(int argc,  char **argv);
int					builtin_cd(int argc, t_ast *node);
int					builtin_pwd(int argc);
int					builtin_export(int argc, t_ast *node);
int					builtin_unset(int argc, t_ast *node);
int					builtin_env(int argc,  char **argv, char **envp);
int					builtin_exit(int argc,  char **argv, char **envp);

/*--Env--*/
t_list				*env_lst_add(t_list **lst, char *str);
t_list				*env_lst_from_str_arr(char **to_copy);
char				**env_lst_to_str_array(t_list *env);
char				*env_get(t_list *env, char *key);
int					env_set(t_list *env, char *key, char *value);
void				env_delete_key(t_list **head, char *key);
void				env_free(void *ptr);

/*---Pipes---*/
int					pipe_create(t_ast *writer, t_ast *reader);
void				pipe_propagate_fd(t_ast *node);
int					pipe_from_file(char *filename);

/*---Traversal--*/
int					traverse_node(t_ast *node);
int					traverse_file(t_ast *node, int flags);
int					traverse_pipe(t_ast *node);
int					traverse_builtin(t_ast *node);
int					traverse_andor(t_ast *node, t_node_type type);
void				andor_propagate_fd(t_ast *node);

/*---Execution---*/
int					exec_process(t_ast *command);
int					exec_builtin(t_ast *builtin);

/* Pratt Parser */
t_ast			*parse(t_token *tokens);
t_ast			*parse_expression(t_parser *parser, t_precedence precedence);
t_ast			*parse_primary(t_parser *parser);
t_ast			*parse_command(t_parser *parser);
t_ast			*parse_subshell(t_parser *parser);
t_precedence	get_precedence(t_token_type type);
int				is_builtin(char *value);

/* Parser utils */
void			advance(t_parser *parser);
int				match(t_parser *parser, t_token_type type);
int				check(t_parser *parser, t_token_type type);
void			error(t_parser *parser, char *message);
int				at_end(t_parser *parser);

/* Environment */
char			*get_env_value(char *key, char **envp);
char			*expand_variables(char *value, char **envp, int last_status, t_quote_type quote);
void			print_tokens(t_token *tokens);

/* Utils */
void			ft_free_tab(char **tab);
void			*ft_realloc(void *ptr, size_t size);

#endif

