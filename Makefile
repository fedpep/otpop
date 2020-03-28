CC = gcc
LD = gcc
RM = rm -f
CP = cp

CFLAGS=-I/usr/include/SDL
LDFLAGS=-lSDL
H_FILES=keyboard.h graph.h character.h motion.h level.h ai.h time.h types.h
OBJ=keyboard.o main.o character.o motion.o level.o ai.o time.o clip_prince.o clip_guard.o graph.o
BIN=otpop

all: $(BIN)

$(BIN): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

%.o: %.c $(H_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

clip_%.c:
	make -C clips


clip_%.o: clip_%.c


clean:
	make -C clips clean
	$(RM) $(BIN) $(OBJ) clip_* *.pyc
