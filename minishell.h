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

typedef enum e_cmd_type
{
	CMD_SIMPLE,
	CMD_PIPE,
	CMD_AND,
	CMD_OR,
	CMD_LIST
}					t_cmd_type;

typedef enum e_reidr_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}					t_redir_type;

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

typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
	struct s_redir	*next;
}					t_redir;

typedef struct s_ast
{
	char			**args;
	t_cmd_type		type;
	t_redir			*redir;
	struct s_ast	*left;
	struct s_ast	*right;
}					t_ast;

/*---Debugs utils---*/

void				print_token(t_token *token);

/*---Tokenizer---*/

t_token				*init_token(char *value, t_token_type type);
void				free_token(t_token *token);
t_token_type		get_token_type(const char *str);
int					ft_isspace(char c);

/*---Lexer---*/

int is_operator_sign(char c);
int is_separator(const char *s);
void add_token(char **tokens, int *count, char *token);
char *in_quote(const char *str);

/*---Ast---*/

t_ast				*init_ast(char **args, t_cmd_type type, t_redir *redir);
t_redir				*init_redir(char *filename, t_redir_type type);
void				free_args(char **args);
void				free_redir(t_redir *redir);
void				free_ast(t_ast *root);

#endif
