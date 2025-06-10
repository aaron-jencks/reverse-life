CC=gcc
CFLAGS=-g

COBJECTS=screen.o grid.o

main: main.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS)

screen.o: screen.c screen.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

grid.o: grid.c grid.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf main $(COBJECTS)