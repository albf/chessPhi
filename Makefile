CFLAGS = -g -Wall -Wextra 

all: alphabeta

alphabeta:
	gcc alpha_beta.c -o alpha_beta $(CFLAGS) 

gdb:
	gdb --args ./alpha_beta
