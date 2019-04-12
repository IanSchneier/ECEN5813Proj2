CC=gcc

CFLAGS=-Wall -Werror -lcunit -Iinc/

SRC := ring.c unittest.c

vpath %.c test/
vpath %.c src/
vpath %.h inc/

OBJ := $(SRC:.c=.o)

unittest: $(OBJ) 
	$(CC) -o unittest $(OBJ) $(CFLAGS)
clean:
	rm -f *.o unittest
