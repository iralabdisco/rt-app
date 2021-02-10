# Makefile

# Flag per il compilatore
BFLAGS=-O0 -ggdb #-O3
CFLAGS=-I. -std=c99 -Wall -Wextra -pedantic
LDFLAGS=-pthread -lm -lrt
DEPS = odo.h schedutil.h
OBJ = main.o odo.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(BFLAGS) $(LDFLAGS)

odo: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(BFLAGS) $(LDFLAGS)

# Target "clean" - Operazione di pulizia
.PHONY: clean

clean:
	rm -rf *.o odo