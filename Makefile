CC = clang

CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = pkg-config


all: encode decode

encode: encode.o huffman.o io.o pq.o node.o code.o stack.o
	$(CC) -o encode encode.o huffman.o io.o pq.o node.o code.o stack.o

decode: decode.o huffman.o io.o node.o code.o stack.o pq.o
	$(CC) -o decode decode.o huffman.o io.o node.o code.o stack.o pq.o


encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

code.o: code.c
	$(CC) $(CFLAGS) -c code.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c

clean:
	rm -f pq.o
	rm -f node.o
	rm -f code.o
	rm -f stack.o
	rm -f io.o
	rm -f huffman.o
	rm -f encode.o encode
	rm -f decode.o decode

format:
	clang-format -i -style=file *.[c,h]

