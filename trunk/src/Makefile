CC=arm-linux-gcc
FILES=./lib/canTransfert.o ./lib/signalForm.o 
FLAGS=-Wall -pedantic

main: main.c $(FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o *~ ./lib/*.o ./lib/*~
