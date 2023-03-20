#
#					Feb 2,1999
#	Makefile for infer/nwsim
#
#
CC = gcc
LIBS   = -lm 
ALTCC     = /opt/SUNWspro/bin/cc
CFLAGS = -O6

LOCAL_HOME = .
SRC  = $(LOCAL_HOME)
UTILS = $(LOCAL_HOME)
INCLUDES = -I$(UTILS)

OBJ_MAIN  = $(SRC)/main.c
OBJ_PERT  = $(SRC)/nw_pert.c

UTILS_DOT_H = $(UTILS)/graph_tools.h \
	$(UTILS)/bit.h \
	$(UTILS)/entropy.h \
	$(UTILS)/stack.h \
	$(UTILS)/pert.h

UTILS_OBJ = $(UTILS)/graph_tools.o \
	$(UTILS)/bit.o \
	$(UTILS)/entropy.o \
	$(UTILS)/stack.o \
	$(UTILS)/pert.o

all: doublepd nw_pert

.SUFFIXES : .o .c

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.c  -c -o $*.o

doublepd: $(OBJ_MAIN) $(UTILS_OBJ) $(UTILS_DOT_H)
	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_MAIN) \
	-o doublepd $(LIBS)

nw_pert: $(OBJ_PERT) $(UTILS_OBJ) $(UTILS_DOT_H)
	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_PERT) \
	-o nw_pert $(LIBS)

clean:
	rm $(UTILS_OBJ)





