CC = gcc
LD = gcc
RM = rm -f

CFLAGS=-I/usr/include/SDL
LDFLAGS=-lSDL -lpthread
H_FILES=keyboard.h graph.h character.h motion.h level.h
OBJ=keyboard.o main.o graph.o character.o motion.o level.o
BIN=fgame

all: $(BIN)

$(BIN): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

%.o: %.c $(H_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(BIN) $(OBJ)
