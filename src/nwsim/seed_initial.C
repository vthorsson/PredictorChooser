//
//
//  seed_initial.C - outputs an unsigned int seed from time function 
// 
// Vesteinn Thorsson and Trey Ideker 2/2/99
#include <iostream.h>
#include <time.h>

main ( void ) {
  
  cout << (unsigned ) time ( NULL ) << endl ;

}



