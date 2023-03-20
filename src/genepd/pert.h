#ifndef __PERT_H_
#define __PERT_H_

#include <stdlib.h>
#include "util.h"

/* define pertType as a 2D array of characters */
typedef char **pertMatrixType;

/* function declarations */
void freePert(pertMatrixType, int);
void putValue(pertMatrixType, int, int, char);
int getValue(pertMatrixType, int, int);
BOOL isPert(pertMatrixType, int, int);
void printPert (pertMatrixType, int, int);

#endif
