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

typedef enum e_quote_type
{
	Q_NONE,
	Q_SIMPLE,
	Q_DOUBLE
}		t_quote_type;	

typedef struct s_lexeme
{
	char *value;
	t_quote_type quote;
}	t_lexeme;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	t_quote_type	quote;
	struct s_token	*next;
}					t_token;

typedef struct s_command
{
	char	*path;
	char	**args;
}	t_command;


typedef struct s_parser
{
	t_token *current;
	t_token *tokens;
}				t_parser;

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


/*---lexer---*/

t_lexeme *lexer(char *line);
void	free_lexemes(t_lexeme *lex);
char *read_line(void);
/*---token--*/

t_token *init_token(const char *line, t_token_type type, t_quote_type quote);
void free_token(t_token *token);
t_token *lexer_to_token(t_lexeme *lex);

/*---ast + utils---*/

void	free_ast(t_ast *ast);
void	free_cmd(t_command *cmd);
t_command *init_cmd(char *path, char **args);
int	count_args(char **args);
t_ast *init_ast(t_node_type type, t_command *command, char *filename, int fd_in, int fd_out);


/*---envp---*/

char *envp(char *key, char **envp);


#endif // MINISHELL_H
