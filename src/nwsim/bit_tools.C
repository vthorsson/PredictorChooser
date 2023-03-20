
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit_tools.h" 
#include "bit.h"

//
//   bit_print.c - Print integer in bit format 
//
// From Trey. Took stderr -> stdout
void bit_print(unsigned v)
{
  int i;
  unsigned mask = 1 << 31;

  for (i=1; i<=32; ++i) {
    putc(((v&mask) == 0) ? '0' : '1', stdout);
    v <<= 1;
    if (i % 8 == 0 && i!=32) putc(' ', stdout);
  }
}
// From Trey. Took stderr -> stdout


//
//                                          May 7, 1998 
//      
//    get_bit.C - gets bit number n from the integer a 
//
//

int get_bit ( int n , unsigned a ) { 

  return   ( ((1<<n) & a)  >> n ) ; 

}

//
//  toggle_bit - toggle bit number n in integer a 
//
unsigned toggle_bit ( int n, unsigned a ) { 

  int b ; 
  unsigned t ; 
  
  b = get_bit ( n, a  ) ; 

  if ( b==0 ){
    t = a | ( 1 << n ) ; 
  }
  else {
    t = a & ( ~(1<<n) ) ; 
  }
  
  return t ; 

}



//
//                                            May 7, 1998
//    
//     link_bits.C - returns integer representation of binary array 
//
//                     input variables: 
//                     b_dim - dimension of binary array 
//                     *b    - pointer to binary array 
//
// 
int link_bits ( int b_dim , int *b  ){ 

  int i, out; 
  
  out = b[ b_dim-1 ] ; 
  for ( i=0 ; i<b_dim-1; i++ ){ 
    out  <<=  1 ;   // shift bits to the left 
    out += b[ b_dim-2-i] ;   // add rightmost bit 
  }
  return out ; 

}

//
//   Checks if function depends on all input variables 
// 

int full_dep ( unsigned func, int nmax ) {

  unsigned i, j, n, unmax;  
  int flag; 

  unmax = (unsigned) nmax ; 

  for ( n=0 ; n<unmax; n++ ) {
    
    flag = 0 ; 
    for ( i=0 ; i<unsigned(1<<unmax) ; i++ ) {
      j = toggle_bit ( n, i ) ;
      if ( get_bit ( i, func )  != get_bit ( j, func ) ) {
	flag = 1 ; 
      }
    }
    if ( flag == 0 ) break ; 
  
  }

  return flag ; 

}

// For Boolean function func of nin input variables
// returns 1 if func is dependent on all nin input varibles
// returns 0 if there is a variable such that output is unchanged
//           when variable is toggled for all possible input states 

int full_Bit_dep ( struct BitVectorType func, int nin ) {

  unsigned i, j, n, unin ;  
  int flag; 

  unin = (unsigned) nin ; 

  for ( n=0 ; n<unin; n++ ) { // for each input variable
    
    flag = 0 ; 
    for ( i=0 ; i< unsigned(1<<unin) ; i++ ) { //loop over all possible inputs
      j = toggle_bit ( n, i ) ;// toggle input n 
      if ( GetBitValue( func, (int)i ) != GetBitValue( func, (int)j ) ) {
	flag = 1 ; // if output has changed then all is well
      }
    }
    if ( flag == 0 ) break ; // if flag=0 there was no change during toggle
    // of n for all possible input states
  
  }

  return flag ; 

}


//
//  char2int_array - Converts character array of bits to
//                   integer array of bits
//         
//                   b is the integer string returned and 
//                   n is the number of bits
//
//  The lowest index ( leftmost ) character becomes the highest
//  index ( n-1 ) in the integer array 


void char2int_array ( char* bit_string, int **b,  int *n ){
  
  int i ;
  char a ; 
  *n = strlen( bit_string ) ; 
  *b = new int [*n]; 

  for ( i=0 ; i< *n ; i++ ){
    a = bit_string[*n-i-1] ;
    if ( a == '*' ) a = '1' ; //  Arbitrarily set 
    //  on the basis that it does not affect deletion matrix 
    (*b)[i] = atoi( &a );
  }

}






