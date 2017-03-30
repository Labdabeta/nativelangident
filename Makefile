.PHONY: clean all debug

all:nli

FLAGS=

debug:FLAGS+=-g
debug:all

nli:nli.c layered_network.o node.o
	gcc $(FLAGS) $^ -o nli -lm

%.o: %.c
	gcc -c $(FLAGS) $^ -o $@

clean:
	rm *.o
