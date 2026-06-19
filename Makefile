CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/cli.c src/gpg.c
OUT = build/alfred

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run:
	./$(OUT)

clean:
	rm -f $(OUT)
