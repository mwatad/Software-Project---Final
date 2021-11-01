FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: cluster

cluster: main.o a.o b.o division.o error.o parser.o
	gcc main.o a.o b.o division.o error.o parser.o -o cluster $(LIBS)

clean:
	rm -rf *.o cluster
	
main.o: main.c error.h parser.h SPBufferset.h
	gcc $(FLAGS) -c main.c
	
a.o: a.c a.h error.h
	gcc $(FLAGS) -c a.c
	
b.o: b.c b.h error.h division.h config.h
	gcc $(FLAGS) -c b.c
	
division.o: division.c error.h division.h
	gcc $(FLAGS) -c division.c
	
error.o: error.c config.h error.h
	gcc $(FLAGS) -c error.c

parser.o: parser.c error.h parser.h
	gcc $(FLAGS) -c parser.c
