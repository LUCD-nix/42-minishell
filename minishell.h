#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <dirent.h> // opendir, readdir, closedir
# include <errno.h>  // errno
# include <fcntl.h>  // open
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>    // signal, sigaction, sigemptyset, sigaddset, kill
# include <stdbool.h>   // bool, true, false
# include <stdio.h>     // printf, perror
# include <stdlib.h>    // malloc, free, exit
# include <string.h>    // strerror
# include <sys/ioctl.h> // ioctl
# include <sys/stat.h>  // stat, lstat, fstat
# include <sys/types.h> // wait, waitpid, wait3, wait4, stat, lstat, fstat,
						// opendir, readdir, closedir
# include <sys/wait.h>  // wait, waitpid, wait3, wait4
# include <termcap.h>   // tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
# include <termios.h>   // tcsetattr, tcgetattr
# include <unistd.h>    // write, access, read, close, fork, dup, dup2, getcwd,
						// chdir, execve, isatty, ttyname, ttyslot
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
	NODE_IN,
	NODE_OUT,
	NODE_APPEND,
	NODE_HEREDOC,
	NODE_FILE
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
	T_QUOTE
}					t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}					t_token;

typedef struct s_command
{
	char	*path;
	char	**args;
}	t_command;

typedef struct s_ast
{
	t_node_type		type;
	t_command		*command;
	char			*filename;
	int				fd_in;
	int				fd_out;
	struct s_ast	*left;
	struct s_ast	*right;
}	t_ast;

/*---Debugs utils---*/

void				print_token(t_token *token);

/*---Tokenizer---*/

t_token				*init_token(char *value, t_token_type type);
void				free_token(t_token *token);
void				add_token(t_token **token, char *value, t_token_type type);
int					is_operator_char(char c);
int					is_operator_len(const char *s);
char				*parse_quote(const char *s, int *i);
int					ft_isspace(char c);
char				*parse_word(const char *s, int *i);
t_token_type		parse_token_type(const char *op);
t_token				*tokenize(const char *line);
char				*parse_operator(const char *s, int *i);

/*---Init structure---*/

t_command 			*init_command(char *path, char **args);
t_ast 				*init_ast(t_node_type type, t_command *cmd, int fd_in, int fd_out);
void 				free_args(char **args);
void				free_cmd(t_command *cmd);
void				free_ast(t_ast *ast);

/*---Builtins---*/

int					is_builtins(t_token *token);
char				*builtins_path(int builtins);
int					builtins_size(t_token *token);
char				**builtins_args(t_token *token);
int					fill_args_from_tokens(t_token *token, char **args, int start);
t_command			*token_to_builtin_cmd(t_token *token);

/*---Tokens to cmd---*/


/*---Built-ins---*/
int					builtin_echo(int argc,  char **argv);
int					builtin_cd(int argc,  char **argv);
int					builtin_pwd(int argc,  char **argv);
int					builtin_export(int argc,  char **argv);
int					builtin_unset(int argc,  char **argv);
int					builtin_env(int argc,  char **argv);
int					builtin_exit(int argc,  char **argv);

#endif // MINISHELL_H
