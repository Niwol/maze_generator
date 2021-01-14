CC = gcc
CFLAGS = -Wall
LDFLAGS = -lSDL2 -lSDL2_ttf

EXEC = mazeGenerator
SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(SRC:.c=.o)

%o: %c 
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o
	rm -f mazeGenerator
