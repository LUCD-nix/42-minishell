NAME = minishell

CFILES := $(wildcard src/*.c)

OBJS := $(CFILES:.c=.o)

CC = cc

LIBFTDIR = libft/

LIBFT := ${LIBFTDIR}libft.a

CFLAGS = -Wall -Wextra -Werror -ggdb3

LFLAGS := -I${LIBFTDIR} -lreadline -lhistory

all: ${NAME}

${NAME}:  ${OBJS} ${LIBFT}
	${CC} ${OBJS} ${LIBFT} ${LFLAGS} -o ${NAME}

%.o: %.c
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
