OBJECTS = $(patsubst src/%.c, bin/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

.PHONY: all clean

all: bin/atcso

bin/%.o: src/%.c $(HEADERS)
	gcc -g -Wall -c $< -o $@

bin/atcso: $(OBJECTS)
	gcc -g -Wall $^ -o $@ -lncurses

clean:
	-rm -f bin/*
