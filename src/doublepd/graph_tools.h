#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "bit.h"
#include "util.h"

typedef struct { 
  int N ;                        /* Number of nodes */
  int **g ;                      /* adjacency matrix */
  struct BitVectorType *out ;    /* node output functions */ 
  int *label ;                   /* Output labels for nodes */
} network; 

network modelf ( char *, BOOL);

void init_model ( network *, int);

void delete_model (network *);

void print_model (network *, FILE *);

int Upper_triangular ( network *, int** ); 

void intrans_get ( int ***, network ) ; 

void get_output ( int* , network, int** ) ; 

void insort ( int**, int, int*, int* ) ; 

void invert ( int*, int*, int ) ; 

#endif
