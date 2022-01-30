CC = gcc
OBJS = quicksort.o
PROG = test

all:	${PROG}

clean:	rm ${OBJS} *~ ${PROG}


${PROG}:	${OBJS}
					${CC} -g -Wall ${OBJS} -o $@

.c.o:		${CC} $< -c -o $@


quicksort.o:	quicksort.c
							gcc -g -c quicksort.c
