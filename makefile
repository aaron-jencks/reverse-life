CC=gcc
CFLAGS=-g

COBJECTS=screen.o grid.o io.o

main: main.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS)

simulator: simulator.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS)

screen.o: screen.c screen.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

grid.o: grid.c grid.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

io.o: io.c io.h grid.o screen.o types.h
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf main simulator $(COBJECTS)