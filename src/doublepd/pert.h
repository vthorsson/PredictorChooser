#ifndef __PERT_H_
#define __PERT_H_

#define LINESIZE = 80;

#include <stdlib.h>
#include "graph_tools.h"
#include "util.h"

/* define pertType as a 2D array of characters */
typedef char **pertMatrixType;

/* function declarations */
pertMatrixType newPert (int, int) ;
void freePert(pertMatrixType, int);
void putValue(pertMatrixType, int, int, char);
int getValue(pertMatrixType, int, int);
BOOL isPert(pertMatrixType, int, int);
void printPert (pertMatrixType, int, int, FILE *);
pertMatrixType getPertList (FILE *, int, int *);
pertMatrixType singlePertList (FILE *, int, network, int **);

#endif
