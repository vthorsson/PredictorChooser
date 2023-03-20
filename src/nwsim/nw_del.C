//
//                                 April 14, 1999
//
//   nw_del.C - generates deletion matrix for given network 
//
//   Vesteinn Thorsson and Trey Ideker  
//
#include <iostream.h>
#include <iomanip.h> 
#include <fstream.h>
#include "graph_struct.h"
#include "graph_tools.h"

main ( int argc, char *argv[] ){

  int i, j, n, *O, *Owt, N, **intrans, **delmat, **intrans_up; 
  struct network net ; 
  struct BitVectorType nstore, tempBV; 

  if ( argc != 3 ){
    cout << "argv[1] = network file (input) " << '\n';
    cout << "argv[2] = deletion matrix (output)" << '\n';
    cout << "nw_del.C v." << __DATE__ << endl ; 
    return(0);
  }
 
  // Read in the model 
  
  modelf ( argv[1], &net );
  N = net.N ; 

  // Initialize 

  intrans_get ( &intrans, net ) ; 
  
  O   = new int[N] ; 
  Owt = new int[N] ; 
  delmat = new int*[N+1] ; 
  for ( i=0 ; i<N+1 ; i++ ) delmat[i] = new int [N] ; 

  // Convert transition matrix to upper triangular form

  Upper_triangular ( &net, intrans ) ;

  // Compute incoming transitions for upper triangular form 

  intrans_get ( &intrans_up, net ) ; 
  
  // Compute wild type output 

  get_output ( O, net, intrans_up ) ; 

  for ( i=0 ; i<N; i++ ) { 
    Owt[i]  = O[i] ; 
    delmat[0][ net.label[i] ] = O[i]  ;
  }

  // Toggle wild-type genes and evaluate network state 

  for ( n=0 ; n<N ; n++ ){ 
    NewBitVector( &nstore, (net.out[n]).NoBits ) ; 
    CopyBitVector( net.out[n], &nstore ) ; 
    if ( Owt[n] == 1  ){  
      ClearBitVector( &net.out[n] ) ; // force output to be low for all inputs
    } else {
      NewBitVector( &tempBV, (net.out[n]).NoBits ) ; 
      ComplementBitVector( tempBV, &net.out[n] ) ; // force output to be high
      KillBitVector( &tempBV ) ; 
    }
    get_output ( O, net, intrans_up ) ; 
    for ( i=0 ; i<N ; i++ ) {
      delmat [ net.label[n]+1 ][ net.label[i] ] = O[i] ; 
    } 
    CopyBitVector( nstore, &net.out[n] ) ; 
    KillBitVector( &nstore ) ; 
    //    net.out[n] = nstore ;    
  }

  // Output Deletion matrix ( perturbation matrix )

  ofstream out( argv[2] ) ; 

  // Wild-type output 
  for ( j=0 ; j<N ; j++ ) {
    out << setw(1) << delmat[0][j] << " " ; 
  } 
  out << endl ; 
  for ( i=1 ; i<N+1 ; i++ ) {
    // Nodes with indices less than i-1
    for ( j=0 ; j<(i-1) ; j++ ) {
      out << setw(1) << delmat[i][j] << " " ; 
    }
    // Denote overexpression of variable by +, deletion by -
    if ( delmat[0][i-1] == 0 ){
      out << "+ " ; 
    } else if ( delmat[0][i-1] == 1 ) {
      out << "- " ;
    } else {
      cout << "Error: Wild type state has symbol other than 0, 1 " << endl ;
      exit(8) ; 
    }
    // Nodes with indices greater than i-1
    for ( j=i ; j<N ; j++ ) {
      out << setw(1) << delmat[i][j] << " " ; 
    }
    out << endl ; 
  }

  out.close() ; 

}








