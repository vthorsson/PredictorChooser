#define LINESIZE 80
#define OUT stdout
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "matrix.h"
#include "stat.h"

/* global variables */
BOOL debug, header;

/* function declarations */
int ** nwRead (FILE *, int *);
statType infoRead (FILE *, int);
BOOL cyclicNet(char *, char *);
void nwCompare(statType *, int **, int **, int, int, int);

