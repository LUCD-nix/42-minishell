/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvanaut <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:55:24 by alvanaut          #+#    #+#             */
/*   Updated: 2025/07/31 14:55:36 by alvanaut         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <readline/readline.h>
#include <readline/history.h>

# include <stdio.h>      // printf, perror
# include <stdlib.h>     // malloc, free, exit
# include <unistd.h>     // write, access, read, close, fork, dup, dup2, getcwd, chdir, execve, isatty, ttyname, ttyslot
# include <fcntl.h>      // open
# include <sys/types.h>  // wait, waitpid, wait3, wait4, stat, lstat, fstat, opendir, readdir, closedir
# include <sys/stat.h>   // stat, lstat, fstat
# include <dirent.h>     // opendir, readdir, closedir
# include <sys/wait.h>   // wait, waitpid, wait3, wait4
# include <signal.h>     // signal, sigaction, sigemptyset, sigaddset, kill
# include <string.h>     // strerror
# include <termios.h>    // tcsetattr, tcgetattr
# include <termcap.h>    // tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
# include <sys/ioctl.h>  // ioctl
# include <errno.h>      // errno
# include <stdbool.h>    // bool, true, false

typedef enum e_exec_type
{
    E_BINARY,
    E_BUILTIN
}   t_exec_type;

typedef enum e_output
{
    O_STDOUT,
    O_PIPE,
    O_FILE
}   t_output;

typedef enum e_input
{
    O_STDIN,
    O_PIPE,
    O_FILE
}   t_input;

typedef enum e_token_type
{
	T_WORD,
	T_COMMAND,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_APPEND,
	T_PIPE,
	T_HEREDOC,
	T_BUILTIN,
	T_EXIT_STATUS
};

typedef struct s_token
{
    s_token *next;
} t_token;

typedef struct s_command
{
    t_exec_type type;
    char        *command;
    char        **args;
    t_input     in;
    t_output    out;
    int         fd_in;
    int         fd_out;
}   t_command;

#endif
