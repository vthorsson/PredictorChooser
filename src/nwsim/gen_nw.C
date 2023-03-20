//
//                                   May 10, 1998
//
//   gen_nw.C - generate random network 
//
//
//   Vesteinn Thorsson and Trey Ideker 
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h> 
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "graph_tools.h"
#include "random.h" 
#include "bit_tools.h"
#include "bit.h"

main ( int argc, char *argv[] ){

  int i, j, n, N, nsource, nin, ninmax ;
  unsigned seed ; 
  int **g , *inputs, *label, *outfunc, *invlabel, *nintrans; 
  struct BitVectorType *out_func ; 
  void invert ( int*, int*, int ) ; 

  if ( argc != 5 && argc != 6 ){
    cout << "argv[1] = New network file" << endl ;
    cout << "argv[2] = Number of nodes" << endl ;
    cout << "argv[3] = Number of sources" << endl ;
    cout << "argv[4] = Maximum no. of inputs to nodes" << endl ;
    cout << "argv[5] = (Optional) Unsigned integer initializing random number generator" << endl ; 
    cout << "gen_nw.C v." << __DATE__ << endl ; 
    return(0);
  }

  ofstream out(argv[1]);
  N = atoi( argv[2] ) ; 
  nsource = atoi ( argv[3] ) ;
  ninmax  = atoi ( argv[4] ) ; 
  if ( argc == 6 ){
    seed  = (unsigned) atoi ( argv[5] ) ;
  } else { 
    seed = (unsigned) time (NULL) ;
  }

  // Initialize 

  g = new int*[N] ; 
  for ( i=0 ; i<N ; i++ ){ 
    g[i] = new int[N] ; 
    for ( j=0 ; j<N ; j++ ) g[i][j] = 0 ; 
  }
  inputs   = new int [N] ; 
  label    = new int [N] ; 
  invlabel = new int [N] ; 
  outfunc  = new int [N] ; 
  out_func = new BitVectorType [N] ; 
  nintrans = new int [N] ; // no.of transitions into a node 

  srand( seed );   // Initialize random no. generator

  // Create graph in upper triangular form  

  out << "num_nodes " << N << endl << endl ; 

  for ( n=0 ; n<nsource ; n++ ) nintrans[n] = 0 ; 
  for ( n=nsource ; n<N ; n++  ){ 
    if ( ninmax < n )
      nin = choose_int ( ninmax ) + 1 ; 
    else 
      nin = choose_int ( n ) + 1 ; 
    nintrans[n] = nin ; 
    if ( nin < n ) {
      choose_ints ( nin, n, inputs ) ; 
      for ( i=0 ; i<nin ; i++  ){ 
	g[ inputs[i] ][n] = 1 ;
      }
    }
    else 
      for ( i=0 ; i<n ; i++ ) g[i][n] = 1 ; 
  }

  /*
  for ( i=0 ; i<N; i++ ){
    for ( j=0 ; j<N ; j++ ){
      out << setw(1) << g[i][j] << " " ; 
    }
    out << endl ; 
  }
  out << endl ; 
  */

  // Generate output functions 

  for ( n=0 ; n<nsource ; n++ ){ 
    NewBitVector( &out_func[n], 1 ) ; 
    BitVectorIns( &out_func[n], 0 ) ; 
  } 
  for ( n=nsource ; n<N ; n++ ) {
    NewBitVector( &out_func[n], 1<<nintrans[n] ); 
    choose_bool_func ( &out_func[n] , nintrans[n] ) ; 
  }

  // Generate node labels 

  choose_ints ( N, N, label ) ; 

  invert ( label, invlabel, N ) ; 

  // Write relabeled network 
  
  out << "Graph" << endl ; 
  
  for ( i=0 ; i<N; i++ ){
    for ( j=0 ; j<N ; j++ ){
      out << setw(1) << g[ invlabel[i] ][ invlabel[j] ] << " " ; 
    }
    out << endl ; 
  }
  out << endl ; 

  // Write relabeled output functions 

  /**** confirm that order is as below 
    for ( i = (1<<nintrans[ invlabel[n]])-1 ; i>= 0 ; i-- ){
      out << get_bit ( i , (unsigned) outfunc [ invlabel[n] ] ) ; 
    }
  ********/

  out << "Functions " << endl ;
  for ( n=0 ; n<N ; n++ ){ 
    out << "[" << n << "]: " ;  
    for ( i=0 ; i < out_func[invlabel[n]].NoBits; i++) {
      //      if (i % 10 == 0) out << " " ; 
      out << GetBitValue ( out_func[invlabel[n]], i);
    }
    out << endl ; 
  }
  out << endl ; 

  out.close() ; 

  return 0  ;

}

