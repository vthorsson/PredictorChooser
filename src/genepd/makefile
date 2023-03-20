# Makefile to compile GenePredict program genepd
# Trey Ideker 6/18/98

EFILE = genepd
CFLAGS = -O6
ALTCC = /opt/SUNWspro/bin/cc
CC = gcc
OBJS = main.o stack.o io.o util.o bit.o BnB.o matrix.o fn.o pert.o

$(EFILE): $(OBJS)
	@echo "linking ..."
	$(CC) $(CFLAGS) -o $(EFILE) $(OBJS) -lm

main.o:  main.c main.h stack.h bit.h io.h matrix.h BnB.h util.h fn.h pert.h
	$(CC) $(CFLAGS) -c $*.c

stack.o: stack.c stack.h bit.h util.h
	$(CC) $(CFLAGS) -c stack.c

io.o: io.c io.h main.h matrix.h
	$(CC) $(CFLAGS) -c io.c

util.o: util.c util.h 
	$(CC) $(CFLAGS) -c util.c

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c

bit.o: bit.c bit.h util.h
	$(CC) $(CFLAGS) -c bit.c

BnB.o: BnB.c BnB.h main.h util.h
	$(CC) $(CFLAGS) -c BnB.c

fn.o:  fn.c fn.h main.h
	$(CC) $(CFLAGS) -c fn.c

pert.o: pert.c pert.h
	$(CC) $(CFLAGS) -c pert.c

clean:
	rm *.o
	rm core
