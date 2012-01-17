CC=arm-linux-gcc
FILES=./can.o ./mcp2515.o
FLAGS=-Wall -pedantic

main: main.c $(FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o *~
