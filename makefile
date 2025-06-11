CC=gcc
CFLAGS=-g

COBJECTS=screen.o grid.o io.o arraylist.o priority.o error.o
TEST_OBJECTS=testing/priority.o testing/priority2.o

main: main.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS)

test: testing/main.c $(TEST_OBJECTS) testing/testing.h $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS) $(TEST_OBJECTS)

simulator: simulator.c $(COBJECTS) types.h
	$(CC) $(CFLAGS) -o $@ $< $(COBJECTS)

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
	rm -rf main simulator $(COBJECTS)