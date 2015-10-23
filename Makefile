OBJECTS = $(patsubst src/%.c, bin/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

.PHONY: all clean

all: bin/atcso

bin/%.o: src/%.c $(HEADERS)
	gcc -c $< -o $@

bin/atcso: $(OBJECTS)
	gcc $^ -o $@

clean:
	-rm -f bin/*
