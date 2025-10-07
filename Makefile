NAME = minishell

CFILES = src/ast/ast_free.c                          \
	src/ast/ast_init.c                               \
	src/ast/parser_commands.c                        \
	src/ast/parser_command_utils.c                   \
	src/ast/parser_command_utils2.c                  \
	src/ast/parser_main.c                            \
	src/ast/parser_main_utils.c						 \
	src/ast/parser_operations.c                      \
	src/ast/parser_redirections.c                    \
	src/ast/parser_redirections_utils.c              \
	src/ast/pratt_parser.c                           \
	src/ast/pratt_parser_utils.c                     \
	src/builtins/builtin_cd_pwd.c                    \
	src/builtins/builtin_echo_env_exit.c             \
	src/builtins/builtin_export_unset.c              \
	src/envp/env.c                                   \
	src/envp/envp.c                                  \
	src/envp/envp_utils.c                            \
	src/envp/env_init_append.c                       \
	src/exec/exec.c                                  \
	src/exec/exec_builtin.c                          \
	src/exit/exit.c                                  \
	src/lexer/lexer_main.c                           \
	src/lexer/lexer_handler.c                        \
	src/lexer/lexer_utils.c                          \
	src/lexer/lexer_handler_utils.c                  \
	src/main.c                                       \
	src/pipes/pipes.c                                \
	src/token/token.c                                \
	src/token/utils.c                                \
	src/traversal/travers_node_redir.c               \
	src/traversal/traversal_heredoc.c                \
	src/traversal/traverse_builtin.c                 \
	src/traversal/traversal_heredoc_utils.c          \
	src/traversal/traversal_heredoc_utils2.c         \
	src/traversal/traversal_heredoc_utils3.c         \
	src/signals/signals_utils.c                      \
	src/signals/signals.c                            \
	src/main_utils.c                                 \
	src/main_utils2.c                                \
	src/traversal/traversal_heredoc_utils4.c         \
	src/exec/exec_utils.c                            \
	src/traversal/traverse_file_pipe_andor.c         \
	src/traversal/traversal_heredoc_utils5.c         \
	src/traversal/traverse_subshell.c                

OBJS := $(CFILES:.c=.o)

CC = cc

LIBFTDIR = libft/

LIBFT := ${LIBFTDIR}libft.a

CFLAGS = -Wall -Wextra -Werror -ggdb3

LFLAGS := -I${LIBFTDIR} -lreadline -lhistory

all: ${NAME}

${NAME}:  ${OBJS} ${LIBFT}
	${CC} ${OBJS} ${LIBFT} ${LFLAGS} -o ${NAME}

%.o: %.c minishell.h
	${CC} -c ${CFLAGS} $< -o $@

${LIBFT}:
	cd ${LIBFTDIR} && make bonus

clean:
	cd ${LIBFTDIR} && make clean
	rm -f ${OBJS}

fclean: clean
	cd ${LIBFTDIR} && make fclean
	rm -f ${NAME}

re: fclean all

.PHONY: fclean clean all re bonus
