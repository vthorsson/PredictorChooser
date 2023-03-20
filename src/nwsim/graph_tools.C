
#include <iostream.h> 
#include <fstream.h>
#include <string.h>

#include "graph_struct.h"
#include "bit_tools.h"
#include "bit.h"
#include "graph_tools.h" 

//            
//				May 7, 1998
//
//   modelf.C - read network from file of the form: num_nodes, Graph, Functions 
//		
//              nw->label left unallocated 
//
//  
void modelf ( char file[], struct network *nw ) 
{
  char word[1000000];// could be improved by reading in char by char
  int i, j, N, n, nn, ntrans=0 ;
  int *int_array ; 

  cout << "Reading network file " << file << endl ; 

  ifstream in(file);
  if ( in.bad() ) {
    cerr << "Unable to open network file\n";
    exit (8);
  }
  
  // Number of nodes 

  in >> word ; 
  while (!strstr(word,"num_nodes") ) in >> word ;  
  in >> nw->N ;
  N =  nw->N ;

  cout << "Number of nodes: " << N << endl ; 

  // Memory allocation 

  nw->g     = new int*[N] ; 
  for ( i = 0 ; i < N ; i++ ) nw->g[i] = new int[N] ; 
  nw->out    = new BitVectorType[N] ;

  // Read graph

  while( !strstr(word,"Graph") || (strlen(word)!=5) ) in >> word ;

  for ( i = 0; i<N; i++ ){
    for ( j = 0; j<N; j++ ){
      in >> nw->g[i][j] ;
      ntrans += nw->g[i][j] ; 
    }
  } 

  cout << "Network has " << ntrans << " out of " 
       << N*(N-1)/2. << " possible transitions" << endl  ;


  // Read node functions as strings and convert to integers  

  while( !strstr(word,"Functions") || (strlen(word)!=9) ) in >> word ;

  for ( n=0 ; n<N ; n++ ) {
    in >>  word ; 
    in >>  word ; 
    char2int_array ( word, &int_array, &nn );
    NewBitVector( &(nw->out[n]), nn ); 
    IntArray2BitVector( &(nw->out[n]), int_array ) ; 
    //    nw->out[n] = link_bits ( nn, int_array );
  }

  in.close();

}


//
//
//  Upper_triangular - Convert DAG to an upper triangular form 
//                     arranging nodes heirarchically: 
//                     First level, sources
//                     Second, genes depending on sources
//                     Third, genes depending on the 2nd set ....
//
//
//                     Here: node array a containing m nodes labels :
//                     dimensions m+1 
//                     a[0] = m 
//                     a[1], ..., a[m] : node labels 
 
void Upper_triangular ( struct network *nw, int** intrans ){

  int i, j, k, m, l, N, found, count; 
  int *up_label;    // New enumaration that gives upper triangular form 
  int *nodes_above; // Nodes array containing nodes found in previous step
  int **a;          // Work copy of transition matrix 
  int *temp;        // Temporary node array storage for nodes_above 
  BitVectorType *temp_func; // Temporary BitVector array for functions

  // Initialize

  N = nw->N ; 
  up_label    = new int [N] ; 
  nw->label   = new int [N] ; 
  nodes_above = new int [N+1] ; 
  a           = new int *[N] ; 
  for ( i=0 ; i<N ; i++ ) a[i] = new int [N] ;  
  temp        = new int [N];
  temp[0]     = 0 ; 
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) a[i][j] = nw->g[i][j] ; 
  }
  temp_func   = new BitVectorType [N] ; 

  // Find sources 
  
  count = 0 ; 
  for ( i=0 ; i<N ; i++ ) {
    if ( intrans[i][0] == 0 ){ 
      up_label[count] = i ; 
      nodes_above[count+1] = i ;
      count++ ;
    }
  }
  nodes_above[0] = count ; 

  // Compute heirarchy of dependencies 

  for ( l=0 ; l<N ; l++ ){

    for ( i=0 ; i< nodes_above[0] ; i++ ){  // for each member of tier
      for ( j=0 ; j<N ; j++ ){  
	if ( a[nodes_above[i+1]][j] == 1 ){   // find node it transitions to 
	  // determine if all nodes transitioning to j are included
	  for ( k=0 ; k< intrans[j][0] ; k++ ){
	    found = 0 ; 
	    for ( m=0 ; m< count ; m++ ){  // look for match in up_label set 
	      if ( up_label[m] == intrans[j][k+1] ) found=1 ;
	    }
	    if ( found== 0 ) break ; // if not found, do not include j 
	  }
	  if ( found == 1 ){  // if all nodes transitioning to j are included 
	    up_label[count] = j ;    // add to label list  
	    for ( k=0 ; k<N ; k++ ) a[k][j] = 0 ; // zero out column j 
	    temp[0]++ ;
	    temp[ temp[0] ] = j ; // store for above list update 
	    count++ ; 
	  }
	}	
	if ( count == N ) break ; 
      }
      if ( count == N ) break ; 
    }

    if ( temp[0] == 0 && count!= N ) {  // No nodes were added and termination not reached
      // Assume this can only be causes by internal cycle in nodes that tier transitions to
      cout << "Error: Upper triangular form not found " << endl ; 
      exit(8) ; 
    } else { 
      // update nodes_above vector 
      nodes_above[0] = temp[0] ; 
      for ( i=0 ; i<=temp[0] ; i++ ) nodes_above[i] = temp[i] ; 
      temp[0] = 0 ; 
    }

  } // end of l loop 

  // Transfer g to temporary location a
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) a[i][j] = nw->g[i][j] ; 
  }

  // Transform transition matrix to upper triangular form  
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) nw->g[i][j] = a[ up_label[i] ][ up_label[j] ] ;
  }

  // Confirm upper triangularity ( and absence of self loops )
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<=i ; j++ ){
      if ( nw->g[i][j] != 0 ){
	cout << "Error: Upper triangular form not found " << endl ; 
	exit(8) ; 
      }
    }
  }

  // Transfer output functions to temporary location
  //  for ( i=0 ; i<N ; i++ ) temp[i] = nw->out[i] ;
  for ( i=0 ; i<N ; i++ ){
    NewBitVector( &temp_func[i], (nw->out[i]).NoBits ) ; 
    CopyBitVector ( nw->out[i], &temp_func[i] ) ; 
  }  

  // Free memory used by old output functions
  for ( i=0 ; i<N ; i++ ) KillBitVector( &(nw->out[i]) ) ; 

  // Arrange output function in upper triangular ordering 
  //  for ( i=0 ; i<N ; i++ ) nw->out[i] = temp[ up_label[i] ] ;
  for ( i=0 ; i<N ; i++ ){
    NewBitVector( &(nw->out[i]), (temp_func[up_label[i]]).NoBits ); 
    CopyBitVector( temp_func[up_label[i]], &(nw->out[i]) ); 
  }

  // Transfer labels 
  for ( i=0 ; i<N ; i++ ) nw->label[i] = up_label[i] ;

  // Deallocate memory 
  delete up_label ; 
  delete nodes_above ; 
  for ( i=0 ; i<N ; i++ ) delete a[i] ; 
  delete a ; 
  delete temp ;
  for ( i=0 ; i<N ; i++ ) KillBitVector( &(temp_func[i]) ) ;
  delete temp_func ; 

  /*****  Optional diagnostic output ******/
  /*******
  cout << "labels " << endl ;
  for ( i=0 ; i<N ; i++ ){
    cout << nw->label[i] << " " ; 
  }
  cout << endl ; 


  cout << " new matrix " << endl ; 
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) cout << nw->g[i][j] << " " ; 
    cout << endl ; 
  }
  cout << endl ; 

  cout << " functions " << endl ; 
  for ( i=0 ; i<N ; i++ ){
    cout << i << " " << nw->out[i] << endl ;   
  }
  cout << endl ; 
  *********/

}

/*******************
//
// expand_array - incoming node array b has length n 
//                take b[n] = a and increment n 
//                Requires preallocated memory 

void expand_array ( int *b , int *n, int a ){

  b[n] = a ; 
  n++ ; 

}

//
// reduce_array  - remove element no. m from array b
//                  elements i > m are shifted downwards
//                  array returns with length n-1
//


void take_from_node_array ( int *b, int *n, int m ){

  int i; 
  for ( i=m+1; i<N ; i++ ){
    b[i-1] = b[i] ; 
  }
  b[N] = 0 ; // arbitrary value, deallocating memory would be more elegant
  
  n = n-1 ; 

}
***********/

//
//                                May 7, 1998
// 
//   intrans_get.C - nonzero transitions into all nodes 
//                   intrans[i][0] - number of nonzero transitions into i 
//                   intrans[i][j] - node numbers for nonzero transitions
//
//

void intrans_get ( int ***intrans, struct network nw ) 
{
  
  int N, i, j, count ; 

  N = nw.N ; 
  *intrans = new int*[N] ;

  // Fill in indices for nonzero transitions 

  for ( j = 0 ; j < N ; j++ ){ 

    count = 0 ;
    for ( i = 0 ; i < N ; i++ ){
      if ( nw.g[i][j] != 0 ) count++ ; 
    }
    (*intrans)[j] = new int[count+1] ;
    count = 0 ; 

    for ( i = 0 ; i < N ; i++ ){
      if ( nw.g[i][j] != 0 ){
	count++ ;
	(*intrans)[j][count] = i ;
      }
      (*intrans)[j][0] = count ; 
    }
  }

}
// 
//   outrans_get.C - nonzero transitions out of nodes 
//                   outtrans[i][0] - number of nonzero transitions out of i 
//                   outrans[i][j] - node numbers for nonzero transitions
//
//

void outtrans_get ( int ***outtrans, struct network nw ) 
{
  
  int N, i, j, count ; 

  N = nw.N ;
  *outtrans = new int*[N] ;

  for ( i = 0 ; i < N ; i++ ){
    count = 0 ; 
    for ( j = 0 ; j < N ; j++ ){
      if ( nw.g[i][j] != 0 ) count++ ;
    }
    (*outtrans)[i] = new int[count+1] ;    
    count = 0 ;
    for ( j = 0 ; j < N ; j++ ){
      if ( nw.g[i][j] != 0 ){
        count++ ;
        (*outtrans)[i][count] = j ;
      }
      (*outtrans)[i][0] = count ; 
    }
  }

}

//
//                                    May 8, 1998
// 
//    get_output.C - gets output of gene network 
//                
//

#define INMAX 8

void get_output ( int *O, struct network nw, int **intrans ) {  

  int n, N, i, no_inputs, *input, *input_node, *invlabel, in, nsrc ; 
  
  N = nw.N ; 
  input       = new int [INMAX] ; 
  input_node  = new int [INMAX] ;      
  invlabel    = new int [N] ; 
  invert ( nw.label, invlabel, N ) ; // ought to be in main

  // Evaluate sources 

  nsrc  =  0 ; 
  while ( intrans[nsrc][0] == 0 ) {
    O[nsrc] = GetBitValue( nw.out[nsrc], 0 ) ; 
    nsrc++ ; 
    if ( nsrc == N ) break ; 
  }

  for ( n=nsrc ; n<N ; n++ ) { 
    
    // Find input nodes 

    no_inputs = intrans[n][0] ; 
    for (  i=0 ; i<no_inputs ; i++ ) { 
      input_node[i] = intrans[n][i+1]   ;
    }

    // Sort input nodes : lowest bit contains lowest (relabeled) node number
 
    insort ( &input_node, no_inputs, nw.label, invlabel ) ; 

    // Find input values corresponding to nodes 

    for (  i=0 ; i<no_inputs ; i++ ) { 
      input[i] = O[ input_node[i] ]   ;
    }
    
    // convert to single integer 

    in = link_bits ( no_inputs, input ) ; 

    // Find output 

    O[n] = GetBitValue( nw.out[n], in ) ; 
    //    O[n]  = get_bit ( in, nw.out[n] ) ; 
  }

  delete input ; 
  delete input_node ; 
  delete invlabel ; 

}

//
// Sort string of input nodes w.r.t. relabeled node numbers 
//

#define INMAX 8 

void insort ( int **a , int m, int *lbl, int *invlbl) {
  
  void up_sort ( int , int** ) ; 
  
  int i, *b;

  b = new int [INMAX] ; 

  for ( i=0 ; i<m ; i++ ) b[i] = lbl[(*a)[i]] ; 

  // sort b from lowest to highest 
  
  up_sort ( m, &b ) ;   

  for ( i=0 ; i<m ; i++ ) (*a)[i] = invlbl[ b[i] ] ; 

  delete b ; 

}
  

//
//       Sort integers from lowest to highest 
//

#define BIG 100000

void up_sort ( int m , int **b ) {

  int i, j, jlow, lowval, temp ; 

  for ( i=0 ; i<m ; i++ ) {

    lowval = BIG  ;    
    for (j=i ; j<m ; j++ ){
      if ( (*b)[j] < lowval ){
	lowval = (*b)[j] ;
	jlow   = j ; 
      }
    }

    if ( jlow != i  ){
      temp       = (*b)[i] ; 
      (*b)[i]    = lowval ; 
      (*b)[jlow] = temp ; 
    }
  }

}


//

void invert ( int *a, int *b, int k ) { 

  int i ; 
  
  for ( i=0 ; i<k ; i++ ) b[ a[i] ] = i ; 

} 







