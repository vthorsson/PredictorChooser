#ifndef __IO_H__
#define __IO_H__

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "util.h"
#include "stack.h"
#include "pert.h"

pertMatrixType ReadPerturbs (FILE *, int *, int *); 
void SaveOutput (FILE *, int, int **, char **);
void SaveOutputAll (char *file_base, int, int, int **, char **);
void SaveStats (char *, stack **, int);

#endif
