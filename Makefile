OBJECTS = $(patsubst src/%.c, bin/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

.PHONY: all clean

all: bin/atcso

bin/%.o: src/%.c $(HEADERS)
	gcc -g -O0 -Wall -Wpedantic -Wextra -c $< -o $@ -std=c99

bin/atcso: $(OBJECTS)
	gcc -g -O0 -Wall -Wpedantic -Wextra $^ -o $@ -lncurses -std=c99

clean:
	-rm -f bin/*
