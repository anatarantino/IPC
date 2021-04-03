CC = gcc
CFLAGS = -Wall -pedantic -g -fsanitize=address -std=c99
CLIBS = -pthread -lrt 
RM = rm -f

SOURCES = $(wildcard *.c)
BINS = $(SOURCES:.c=.out)

TESTF = Archive/jnh/*

all: $(BINS)

%.out: %.c
	$(CC) $(CFLAGS) $(CLIBS) $^ -o $@ 

clean:
	$(RM) $(BINS)

clean_test:
	$(RM) 