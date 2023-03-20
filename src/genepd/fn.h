#ifndef __FN_H_
#define __FN_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "main.h"
#include "stack.h"
#include "bit.h"
#include "util.h"

void MinFn(stack **, pertMatrixType, int, int, char *);
void MinFnRand(stack **, pertMatrixType, int, int, char *);
void MinFnAll(stack **, pertMatrixType, int, int, char *);
void build_net(stack **, pertMatrixType, int **, char **, int, 
	       int, int, char *, int *);
void addSoln (BitVectorType, pertMatrixType, int **, char **, int, int, int);
void clearSoln (int **, char **, int, int);

#endif
