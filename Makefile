CC = gcc
CFLAGS = -Wall -pedantic -g -fsanitize=address -std=c99
CLIBS = -pthread -lrt 
RM = rm -rf

SOURCES = $(wildcard *.c)
BINS = $(SOURCES:.c=.out)

TESTF = CBS_k3_n100_m403_b10/*

all: $(BINS)

%.out: %.c
	$(CC) $(CFLAGS) $(CLIBS) $^ -o $@ 

clean: clean_test
	$(RM) $(BINS)

clean_test:
	$(RM) report.tasks PVS-Studio.log strace_out cppoutput.txt *.valgrind

test: clean
	pvs-studio-analyzer trace -- make  > /dev/null
	pvs-studio-analyzer analyze > /dev/null 2> /dev/null
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log  > /dev/null
	valgrind --leak-check=full -v ./solve.out $(TESTF)  2> solveout.valgrind; valgrind --leak-check=full -v ./vista.out  2> vistaout.valgrind; cppcheck --quiet --enable=all --force --inconclusive . 2> cppoutput.txt

.PHONY: all clean test clean_test