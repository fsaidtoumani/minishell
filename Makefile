CC ?= gcc -g
CFLAGS ?= -Wall -O2
LDFLAGS ?=

VPATH=src

objects := builtin.o main.o parser.o cmd.o localvariables.o

.PHONY: clean

minishell: $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

builtin.o: builtin.c builtin.h cmd.h localvariables.o
	$(CC) $(CFLAGS) -c $<

main.o: main.c parser.h cmd.h localvariables.o
	$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h cmd.h localvariables.o
	$(CC) $(CFLAGS) -c $<

processus.o: cmd.c cmd.h builtin.h localvariables.o
	$(CC) $(CFLAGS) -c $<
localvariables.o: localvariables.c localvariables.h
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f $(objects)