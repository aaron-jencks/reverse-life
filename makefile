CC=gcc
CFLAGS=-g
LD=-lz3

COBJECTS=screen.o grid.o io.o arraylist.o priority.o error.o csp.o
TEST_OBJECTS=testing/priority.o testing/priority2.o

main: main.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS) $(LD)

test: testing/main.c $(TEST_OBJECTS) testing/testing.h $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS) $(TEST_OBJECTS) $(LD)

simulator: simulator.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS) $(LD)

screen.o: screen.c screen.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

grid.o: grid.c grid.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

io.o: io.c io.h grid.o screen.o types.h
	$(CC) $(CFLAGS) -o $@ -c $<

arraylist.o: arraylist.c arraylist.h error.o
	$(CC) $(CFLAGS) -o $@ -c $<

priority.o: priority.c priority.h arraylist.o
	$(CC) $(CFLAGS) -o $@ -c $<

error.o: error.c error.h
	$(CC) $(CFLAGS) -o $@ -c $<

testing/priority.o: testing/priority.c arraylist.o priority.o
	$(CC) $(CFLAGS) -o $@ -c $<

testing/priority2.o: testing/priority2.c arraylist.o priority.o
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf main simulator test $(COBJECTS) $(TEST_OBJECTS)