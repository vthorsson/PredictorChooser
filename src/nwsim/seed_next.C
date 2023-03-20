//
//
//      seed_next.C - outputs unsigned int seed given an unsigned int seed 
//
// Vesteinn Thorsson and Trey Ideker 2/2/98

#include <iostream.h>
#include <time.h>
#include <stdlib.h>

main ( int argc, char *argv[] ) {
  
  unsigned seed ; 

  seed = (unsigned) atoi ( argv[1] ) ;  
  srand( seed ) ;

  cout << rand() << endl ; 

}



