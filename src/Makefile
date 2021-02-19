# Makefile

# Flag per il compilatore
BFLAGS=-O0 -ggdb #-O3
CFLAGS=-I. -std=c99 -Wall -Wextra -pedantic
LDFLAGS=-pthread -lm -lrt
DEPS = odo.h schedutil.h
OBJ = main.o odo.o
APPNAME = rt-app.x86_64

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(BFLAGS) $(LDFLAGS)

$(APPNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(BFLAGS) $(LDFLAGS)

# Target "clean" - Operazione di pulizia
.PHONY: clean

clean:
	rm -rf *.o $(APPNAME)