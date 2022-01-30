
FLAGS  = -Wall -g
CC     = gcc
PROG   = sem_test
OBJS   = sem_test.o semlib.o

all:	${PROG}

clean:
	rm ${OBJS} ${PROG}
  
${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@

.c.o:
	${CC} ${FLAGS} $< -c

##########################

semlib.o: semlib.h semlib.c

sem_test.o: semlib.h sem_test.c

sem_test: semlib.o sem_test.o

