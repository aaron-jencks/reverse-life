CC=gcc
CFLAGS=-g

COBJECTS=screen.o

main: main.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS)

screen.o: screen.c screen.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<