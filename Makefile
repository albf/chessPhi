CFLAGS = -g -Wall -Wextra -lpthread -lm

all: alpha_beta

alpha_beta:
	gcc alpha_beta.c -o alpha_beta $(CFLAGS)
gdb:
	gdb --args ./alpha_beta
