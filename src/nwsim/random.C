
#include <stdlib.h>
#include "bit_tools.h"
#include "random.h" 
#include "bit.h"

#include <iostream.h> 

//
//
//  choose_ints.C - draw m integers from 0 to n-1 
//

void choose_ints ( int m , int n, int *a  ){
  
  int i, k, *b ; 
  int choose_int ( int ) ; 

  b = new int [n] ; 
  for ( i=0 ; i<n ; i++ ) b[i] = i ; 

  for ( i=0 ; i<m ; i++ ) {
    k = choose_int ( n-i ) ; 
    a[i] = b[k] ; 
    if ( k != n-1-i  ) b[k]=b[n-1-i] ; 
  }
  
  delete b ; 

}


//
//   choose_int.C - draw integer from 0 to n-1 
//


int choose_int ( int n  ){
  
  int i ; 
  float x, ss ; 
  
  x = float(rand())/float(RAND_MAX);

  i=0 ; 
  ss = 1./(float)n ; 
  
  while ( x>ss ){
    ++i ; 
    ss += 1./(float)n ; 
  }

  return i ; 

}

unsigned choose_unsigned ( int n ) {

  unsigned int i; 
  float x, ss ; 
  
  x = float(rand())/float(RAND_MAX);

  i=0 ; 
  ss = 1./(float)n ; 
  
  while ( x>ss ){
    ++i ; 
    ss += 1./(float)n ; 
  }

  return i ; 

}

void RandomBitVector ( struct BitVectorType *bv ){
  
  int i, n_remain ;
  unsigned int mask ; 
  const int NoBitsUInt = sizeof(unsigned int) * 8;

  ClearBitVector ( bv ) ; 

  /* choose random int for all but last Unit */
  for ( i=0 ; i< ( bv->NoUnits -1 ) ; i++ ) {
    bv->vector[i] = unsigned(rand()+rand()) ; 
    /** This could be improved slightly ,                          ***/ 
    /** since RAND_MAX = INT_MAX  < U_INT_MAX                      ***/
    /** E.g. INT_MAX = 2147483647 = 2**31-1                        ***/
    /** UINT_MAX = 4294967295 = 2**32-1                            ***/
    /** 0 <= rand()+rand() <= 2**32-2                              ***/
    /** NoBitsUInt = 32, Linux,  Unix                              ***/
  }
  
  n_remain = bv->NoBits - ( bv->NoUnits -1 )*NoBitsUInt ; /*Bits in final Unit*/ 
  /*  choose random unsigned int for last unit  */
  if ( n_remain == NoBitsUInt ) { /* last Unit uses all NoBitsUInt */
    bv->vector[ bv->NoUnits -1 ] = rand() + rand() ; 
  } else { /* Bits in final Unit < NoBitsUInt */
    mask = (1 << n_remain) - 1  ;
    bv->vector[ bv->NoUnits -1 ] = unsigned(rand()) & mask ;  
  }

}


//
//  choose_func.C - choose a function depending on all nin input variables 
//
//        
int choose_func ( int nin ) {

  int m, fmax ; 
  
  fmax = (1<<nin) << nin ; 
  m = choose_int ( fmax ) ; 
  while ( full_dep ( (unsigned)m, nin ) == 0 ) m = choose_int ( fmax ) ;

  return m ; 

}

void choose_bool_func ( struct BitVectorType *bv, int nin ){

  RandomBitVector( bv ) ;
  while ( full_Bit_dep( *bv, nin ) == 0 )RandomBitVector (bv) ; 

}

  













