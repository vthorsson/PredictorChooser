#
#					Feb 2,1999
#	Makefile for infer/nwsim
#
#

CC     = g++
LIBS   = -lm 
CFLAGS = -O6

LOCAL_HOME = .
SRC  = $(LOCAL_HOME)
UTILS = $(LOCAL_HOME)
INCLUDES = -I$(UTILS)

OBJ_ALL_GEN  = $(SRC)/gen_nw.C
OBJ_ALL_DEL  = $(SRC)/nw_del.C
# OBJ_ALL_MANY = $(SRC)/nw_del_many.C
OBJ_ALL_DV   = $(SRC)/nw2dv.C 
OBJ_ALL_SI   = $(SRC)/seed_initial.C 
OBJ_ALL_SN   = $(SRC)/seed_next.C 

UTILS_DOT_H = $(UTILS)/graph_struct.h \
	$(UTILS)/bit_tools.h \
	$(UTILS)/graph_tools.h \
	$(UTILS)/random.h \
	$(UTILS)/bit.h

UTILS_OBJ = $(UTILS)/bit_tools.o \
	$(UTILS)/graph_tools.o \
	$(UTILS)/random.o \
	$(UTILS)/bit.o

all: gen_nw nw_del nw2dv seed_initial seed_next
.SUFFIXES : .o .C
.C.o:
	$(CC) $(CFLAGS) $(INCLUDES) $*.C -c -o $*.o

gen_nw: $(OBJ_ALL_GEN) $(UTILS_OBJ) $(UTILS_DOT_H)
	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_ALL_GEN) \
	-o gen_nw $(LIBS)

nw_del: $(OBJ_ALL_DEL) $(UTILS_OBJ) $(UTILS_DOT_H)
	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_ALL_DEL) \
	-o nw_del $(LIBS)

# nw_del_many: $(OBJ_ALL_MANY) $(UTILS_OBJ) $(UTILS_DOT_H)
#	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_ALL_MANY) \
#	-o nw_del_many $(LIBS)

nw2dv: $(OBJ_ALL_DV) $(UTILS_OBJ) $(UTILS_DOT_H)
	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_ALL_DV) \
	-o nw2dv $(LIBS)

seed_initial: $(OBJ_ALL_SI) $(UTILS_OBJ) $(UTILS_DOT_H)
	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_ALL_SI) \
	-o seed_initial $(LIBS)

seed_next: $(OBJ_ALL_SN) $(UTILS_OBJ) $(UTILS_DOT_H)
	$(CC) $(CFLAGS) $(INCLUDES) $(UTILS_OBJ) $(OBJ_ALL_SN) \
	-o seed_next $(LIBS)

clean_all:
	rm $(UTILS_OBJ)





