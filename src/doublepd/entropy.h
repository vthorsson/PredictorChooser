#include <math.h>
#include "stack.h"
#include "bit.h"
#include "util.h"
#include "graph_tools.h"

typedef struct {
  int pert[2];       /* array of perturbed nodes */
  char dir[2];        /* array of pert directions for each node */
  int states;      /* number of distinct states */
  float value;     /* entropy value */
} entropy;

void getStateUT (int, int, char, char, network, int **, int *);

void getEntropy (int **, int, int, entropy *, BOOL);

int entropySort (const void *, const void *);
