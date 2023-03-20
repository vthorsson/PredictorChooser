/* Program genepd    Jan 1999 */
/* Copyright (c) Trey Ideker and Vesteinn Thorsson */

#ifndef __MAIN_H__
#define __MAIN_H__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "bit.h"
#include "matrix.h"
#include "io.h"
#include "BnB.h"
#include "util.h"
#include "fn.h"

/* global variables */
BOOL debug;           /* debug mode flag */
BOOL all_outputs;     /* flag to output all alternate solns */
BOOL limit;           /* limit number of solns to limit_num */
int limit_num;        /* num of solns to limit to */
BOOL rand_outputs;    /* flag to output random sampling of solns */
int rand_num;         /* num of rand samples to take */

/* function declarations */
int ** Perturb2DAG (pertMatrixType, int, int);
int ** FindClosure(int **, int);
stack ** MinInputs (pertMatrixType, int **, int, int);

#endif
