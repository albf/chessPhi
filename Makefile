CFLAGS = -g -Wall -Wextra 

all: serial parallel

serial:
	gcc alpha_beta.c -o alpha_beta $(CFLAGS) -pthread

parallel:
	gcc alpha_beta_parallel.c -o alpha_beta_parallel $(CFLAGS) -lpthread 
gdb:
	gdb --args ./alpha_beta
