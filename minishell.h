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
	T_RPAREN
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
t_token_type		get_token_type(const char *str);
int					ft_isspace(char c);

/*---Lexer---*/
int					is_operator_sign(char c);
int					is_separator(const char *s);
void				add_token(char **tokens, int *count, char *token);
char				*in_quote(const char *str);

/*---Built-ins---*/
int					builtin_echo(int argc,  char **argv);
int					builtin_cd(int argc,  char **argv);
int					builtin_pwd(int argc,  char **argv);
int					builtin_export(int argc,  char **argv);
int					builtin_unset(int argc,  char **argv);
int					builtin_env(int argc,  char **argv);
int					builtin_exit(int argc,  char **argv);

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

#endif
