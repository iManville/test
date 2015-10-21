# Makefile

PROGRAM = insfile
OBJS = insfile.o
CC = gcc

CFLAGS = -Wall
PARAM = `mysql_config --cflags --libs` 

RM = rm -rf

.SUFFIXES: .c .o

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(PARAM) $^

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) $(PROGRAM) $(OBJS)
