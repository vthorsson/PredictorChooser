# Makefile to compile prediction performance evaluation
# program evalpd
# Trey Ideker 1/20/99

EFILE = evalpd
CFLAGS = -O6
CC = gcc
OBJS = main.o util.o matrix.o stat.o

$(EFILE): $(OBJS)
	@echo "linking ..."
	$(CC) $(CFLAGS) -o $(EFILE) $(OBJS) -lm

main.o:  main.c main.h matrix.h util.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c


stat.o: stat.c stat.h main.h
	$(CC) $(CFLAGS) -c stat.c
