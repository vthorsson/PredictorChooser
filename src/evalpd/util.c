/* utility functions */
#include "util.h"

/* generic error handling function */
void die (int error_code) {
  
  char *usage = "Usage: evalpd -d -h <pred file base>";  

  switch (error_code) {

  case 0: 
    fprintf(stderr, "\nMemory allocation failure.\n");
    break;
  case 1: 
    fprintf(stderr, "\nCommand line error.\n  %s\n\n", usage);
    break;
  case 2: 
    fprintf(stderr, "\nCannot read input file\n");
    break;
  case 3:
    fprintf(stderr, "\nNumber out of range\n");
    break;
  default: 
    fprintf(stderr, "\nUnknown error.\n");  
    break;
  }
  fprintf(stderr, "Aborting program execution\n");
  exit(1);
}


