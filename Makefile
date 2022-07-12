CC = gcc
CFLAGS = -Wall -ansi -pedantic 
PROG = prog
SRC= $(wildcard *.c)
OBJ= $(SRC: .c=.o)


all : $(PROG) doxygen

$(PROG) : $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lm

%.o : %.c
	$(CC) $(CFLAGS) -c $(SRC) -o $@ -lm

.PHONY : clean

clean :
	rm -f $(OBJ)

doxygen :
	@doxygen -g
	@doxygen Doxyfile