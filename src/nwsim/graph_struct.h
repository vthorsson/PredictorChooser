#include "bit.h"

struct network { 
  int N ;                        // Number of nodes 
  int **g ;                      // adjacency matrix 
  struct BitVectorType *out ;    // node output functions
  int *label ;                   // Output labels for nodes 
} ; 
