CC = gcc
OBJS = odd_numbers.o
PROG = test

all:	${PROG}

clean:	rm ${OBJS} *~ ${PROG}


${PROG}:	${OBJS}
					${CC} -g -Wall ${OBJS} -o $@

.c.o:		${CC} -g -Wall $< -c -o $@


odd_numbers.o:	odd_numbers.c
