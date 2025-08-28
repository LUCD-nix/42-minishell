NAME = minishell

CFILES = src/ast/ast.c \
	src/ast/parser.c \
	src/ast/pratt_parser.c \
	src/builtins/builtin_cd_pwd.c \
	src/builtins/builtin_echo_env.c \
	src/builtins/builtin_export_unset.c \
	src/envp/env.c \
	src/envp/envp.c \
	src/envp/env_init_append.c \
	src/exec/exec.c \
	src/lexer/lexer.c \
	src/main.c \
	src/pipes/pipes.c \
	src/token/token.c \
	src/token/utils.c \
	src/traversal/traversal.c \
	src/traversal/traversal_funcs.c

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
