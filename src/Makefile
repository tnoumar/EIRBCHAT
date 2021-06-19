ALL: default 

CC           = gcc
CLINKER      = $(CC)
OPTFLAGS     = -O0


SHELL = /bin/sh

CFLAGS  =   -DREENTRANT -Wunused -Wall -g 
CCFLAGS = $(CFLAGS)
LIBS =  -lpthread 

INCLUDES= -I ./inc/
EXECS = client_exec server_exec
default: $(EXECS)

client_exec: client_poll.c client_utils.o
	$(CLINKER) $(OPTFLAGS)  -o client_exec client_poll.c client_utils.o $(LIBS)
	mv client_exec ./bin/
	mv client_utils.o ./bin/
	
server_exec: server_poll2.c server_utils.o
	$(CLINKER) $(OPTFLAGS)  -o server_exec server_poll2.c server_utils.o $(LIBS)
	mv server_exec ./bin/
	mv server_utils.o ./bin/
		
		
clean:
	@-/bin/rm -f *.o *~ PI* $(EXECS) *.out core 
	rm -rf ./bin/*
.c:
	$(CC) $(CFLAGS)   -o $* $< $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -c $<
.o:
	${CLINKER} $(OPTFLAGS)  -o $* $*.o $(LIBS)
