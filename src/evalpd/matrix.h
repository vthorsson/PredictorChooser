#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <stdlib.h>
#include <stdio.h>

int mat_prod(int, int **, int **, int **);
int mat_or(int, int **, int **, int **);
int mat_and(int, int **, int **, int **);
int ** FindClosure(int **, int);
void printMatrix(int **, int, int);

#endif
