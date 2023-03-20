/* graph_tools.c                     */
/* May 7, 1998 by Vesteinn Thorsson  */
/*                                   */
/* converted from C++ to ANSI C      */
/* June 1, 1999 by Trey Ideker       */

#include "graph_tools.h" 

/*************************************************************/
/*   modelf: read network from file of the form:             */
/*           num_nodes, Graph, Functions                     */
/*                                                           */
/*              nw->label left unallocated                   */

network modelf ( char *file, BOOL debug ) 
{
  char line[1000] = "initialization string";  
  int i, j, N, n, nn, ntrans=0 ;
  int *int_array ; 
  FILE *fp;
  network nw;

  if (debug == TRUE) printf("Reading network file %s\n", file) ; 

  fp = fopen(file, "r");
  if ( fp == NULL ) {
    printf("Unable to open network file\n");
    exit(1);
  }

  /* look for num_nodes tag to get size of matrix */
  while ( strncmp( line, "num_nodes", 9) != 0 ) {
    fscanf(fp, "%s", line);
  }  
  fscanf(fp, "%d", &N);
  if (debug == TRUE) printf("Number of nodes: %d\n", N);

  /* Memory allocation */
  init_model(&nw, N);

  /* look for Graph tag to get matrix itself*/
  while (strcmp(line, "Graph") != 0 ) fscanf(fp, "%s", line);
  for ( i = 0; i<N; i++ ){
    for ( j = 0; j<N; j++ ){
      fscanf(fp,"%d", &nw.g[i][j] );
      ntrans += nw.g[i][j] ;       
    }
  }
  if (debug == TRUE) printf("Net has %d out of %d possible transitions\n", 
			    ntrans, N*(N-1)/2); 

  /* Read node functions as strings and convert to integers */
  while (strcmp(line, "Functions") != 0) fscanf(fp, "%s", line);
  for ( n=0 ; n<N ; n++ ) {
    fscanf(fp, "%s", line);
    fscanf(fp, "%s", line);
    nn = strlen( line ) ; 
    int_array = calloc(nn, sizeof(int));
    char2int_array ( line, int_array, nn );
    NewBitVector( &nw.out[n], nn ); 
    IntArray2BitVector( &nw.out[n], int_array ) ; 
    free(int_array);
  }

  /* clean up */
  fclose(fp);
  return(nw);
}

/******************************************************************/
/* initialize a network model given a node number N               */

void init_model ( network *nw, int N ) {

  int i;

  nw->g = calloc (N, sizeof(int *));
  nw->out = calloc (N, sizeof(BitVectorType) );
  nw->label = calloc (N, sizeof(int));
  if (nw->g == NULL || nw->out == NULL || nw->label == NULL) 
    exit(1);
  for ( i = 0 ; i < N ; i++ ) {
    nw->g[i] = calloc (N, sizeof(int));
    if (nw->g[i] == NULL) exit(1);
  }

  nw->N = N;

}

/******************************************************************/
/* delete (free) a network model                                  */

void delete_model ( network *nw ) {

  int i, N;

  N = nw->N;
  for (i=0; i<N; i++) {
    free(nw->g[i]);
    KillBitVector(&nw->out[i]);
  }
  free(nw->g);
  free(nw->label);
  free(nw->out);

}

/********************************************************************/
/* prints a model : added 5/27/99 by Trey Ideker                    */

void print_model ( network *nw, FILE *fp ) {

  int i, j, N;

  N = nw->N;
  fprintf(fp, "num_nodes %d\n\n", nw->N);

  /* fprintf(fp, "labels\n");
   * for ( i=0 ; i<N ; i++ ) fprintf(fp, "%d ", nw->label[i]);
   * fprintf(fp, "\n");
   */

  fprintf(fp, "Graph\n");
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) fprintf(fp, "%d ", nw->g[i][j]);
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");

  fprintf(fp, "Functions\n");
  for ( i=0 ; i<N ; i++ ){
    fprintf(fp, "[%d]:", i);
    fflush(fp);
    FilePrintBitVector(nw->out[i], fp);
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
}

/********************************************************************/
/*  Upper_triangular - Convert DAG to an upper triangular form 
 *                     arranging nodes heirarchically: 
 *                     First level, sources
 *                     Second, genes depending on sources
 *                     Third, genes depending on the 2nd set ....
 *
 *                     Here: node array a containing m nodes labels :
 *                     dimensions m+1 
 *                     a[0] = m 
 *                     a[1], ..., a[m] : node labels 
 *
 *    returns 0 if completed OK, 1 if error: Trey Ideker 5/28/99 */

int Upper_triangular ( network *nw, int** intrans ){

  int i, j, k, m, l, N, found, count; 
  int *up_label;    /* New enumaration that gives upper triangular form */
  int *nodes_above; /* Nodes array containing nodes found in previous step */
  int **a;          /* Work copy of transition matrix */
  int *temp;        /* Temporary node array storage for nodes_above */
  BitVectorType *temp_func; /* Temporary BitVector array for functions */

  /* Initialize */
  N = nw->N ; 
  up_label    = calloc(N, sizeof(int));
  nodes_above = calloc(N+1, sizeof(int));
  a           = calloc(N, sizeof(int *));
  for ( i=0 ; i<N ; i++ ) a[i] = calloc(N, sizeof(int));
  temp        = calloc(N, sizeof(int));
  temp[0]     = 0 ; 
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) a[i][j] = nw->g[i][j] ; 
  }
  temp_func   = calloc(N, sizeof(BitVectorType));

  /* Find sources */
  count = 0 ; 
  for ( i=0 ; i<N ; i++ ) {
    if ( intrans[i][0] == 0 ){ 
      up_label[count] = i ; 
      nodes_above[count+1] = i ;
      count++ ;
    }
  }
  nodes_above[0] = count ; 

  /* Compute heirarchy of dependencies */
  for ( l=0 ; l<N ; l++ ){
    for ( i=0 ; i< nodes_above[0] ; i++ ){  /* for each member of tier */
      for ( j=0 ; j<N ; j++ ){  
	if ( a[nodes_above[i+1]][j] == 1 ){ /* find node it transitions to */
	  /* determine if all nodes transitioning to j are included */
	  for ( k=0 ; k< intrans[j][0] ; k++ ) {
	    found = 0 ; 
	    for ( m=0 ; m< count ; m++ ){ /* look for match in up_label set */
	      if ( up_label[m] == intrans[j][k+1] ) found=1 ;
	    }
	    if ( found == 0 ) break ; /* if not found, do not include j */
	  }
	  if ( found == 1 ){  /* if all nodes into j are included */
	    up_label[count] = j ;    /* add to label list */
	    for ( k=0 ; k<N ; k++ ) a[k][j] = 0 ; /* zero out column j */
	    temp[0]++ ;
	    temp[ temp[0] ] = j ; /* store for above list update */
	    count++ ; 
	  }
	}	
	if ( count == N ) break ; 
      }
      if ( count == N ) break ; 
    }

    if ( temp[0] == 0 && count!= N ) {  
      /* No nodes were added and termination not reached. Assume this can */
      /* only be caused by internal cycle in nodes that tier transitions */
      /* to.  In this case, return to caller with error code */
      return(1) ; 
    } else { 
      /* update nodes_above vector */ 
      nodes_above[0] = temp[0] ; 
      for ( i=0 ; i<=temp[0] ; i++ ) nodes_above[i] = temp[i] ; 
      temp[0] = 0 ; 
    }
  } /* end of l loop */

  /* Transfer g to temporary location a */
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) a[i][j] = nw->g[i][j] ; 
  }

  /* Transform transition matrix to upper triangular form  */
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<N ; j++ ) nw->g[i][j] = a[ up_label[i] ][ up_label[j] ] ;
  }

  /* Confirm upper triangularity ( and absence of self loops ) */
  for ( i=0 ; i<N ; i++ ){
    for ( j=0 ; j<=i ; j++ ){
      if ( nw->g[i][j] != 0 ){
	return(1) ;     /* return to caller with error code */
      }
    }
  }

  /*  Transfer output functions to temporary location */
  for ( i=0 ; i<N ; i++ ){
    NewBitVector( &temp_func[i], (nw->out[i]).NoBits ) ; 
    CopyBitVector ( nw->out[i], &temp_func[i] ) ; 
  }  

  /* Free memory used by old output functions */
  for ( i=0 ; i<N ; i++ ) KillBitVector( &(nw->out[i]) ) ; 

  /* Arrange output function in upper triangular ordering  */
  for ( i=0 ; i<N ; i++ ){
    NewBitVector( &(nw->out[i]), (temp_func[up_label[i]]).NoBits ); 
    CopyBitVector( temp_func[up_label[i]], &(nw->out[i]) ); 
  }

  /* Transfer labels */
  for ( i=0 ; i<N ; i++ ) nw->label[i] = up_label[i] ;

  /* Deallocate memory */
  free(up_label) ; 
  free(nodes_above) ; 
  for ( i=0 ; i<N ; i++ ) free( a[i] ) ; 
  free(a) ; 
  free(temp) ;
  for ( i=0 ; i<N ; i++ ) KillBitVector( &(temp_func[i]) ) ;
  free(temp_func) ; 
  return(0);
}

/***********************************************************************/
/*                                May 7, 1998
 * 
 *   intrans_get.C - nonzero transitions into all nodes 
 *                   intrans[i][0] - number of nonzero transitions into i 
 *                   intrans[i][j] - node numbers for nonzero transitions
 *
 */

void intrans_get ( int ***intrans, network nw ) 
{
  
  int N, i, j, count ; 

  N = nw.N ; 
  *intrans = calloc(N, sizeof(int *)); 

  /* Fill in indices for nonzero transitions */
  for ( j = 0 ; j < N ; j++ ){ 
    count = 0 ;
    for ( i = 0 ; i < N ; i++ ){
      if ( nw.g[i][j] != 0 ) count++ ; 
    }
    (*intrans)[j] = calloc(count+1, sizeof(int));
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

/*********************************************************************
 *                                    May 8, 1998
 * 
 *    get_output.C - gets output of gene network 
 *                
 */

#define INMAX 8

void get_output ( int *O, network nw, int **intrans ) {  

  int n, N, i, no_inputs, *input, *input_node, *invlabel, in, nsrc ; 
  
  N = nw.N ; 
  input       = calloc(INMAX, sizeof(int));
  input_node  = calloc(INMAX, sizeof(int));
  invlabel    = calloc(N, sizeof(int));
  invert ( nw.label, invlabel, N ) ; /* ought to be in main */

  /* Evaluate sources */
  nsrc  =  0 ; 
  while ( intrans[nsrc][0] == 0 ) {
    O[nsrc] = GetBitValue( nw.out[nsrc], 0 ) ; 
    nsrc++ ; 
    if ( nsrc == N ) break ; 
  }

  for ( n=nsrc ; n<N ; n++ ) { 
    
    /* Find input nodes */
    no_inputs = intrans[n][0] ; 
    for (  i=0 ; i<no_inputs ; i++ ) { 
      input_node[i] = intrans[n][i+1]   ;
    }

    /* Sort input nodes: lowest bit contains lowest (relabeled) node num */
     insort ( &input_node, no_inputs, nw.label, invlabel ) ; 

     /* Find input values corresponding to nodes */
    for (  i=0 ; i<no_inputs ; i++ ) { 
      input[i] = O[ input_node[i] ]   ;
    }
    
    /* convert to single integer */
    in = link_bits ( no_inputs, input ) ; 

    /* Find output */
    O[n] = GetBitValue( nw.out[n], in ) ; 
  }
  free(input) ; 
  free(input_node) ; 
  free(invlabel) ; 
}

/***********************************************************************/
/* Sort string of input nodes w.r.t. relabeled node numbers            */

#define INMAX 8 

void insort ( int **a , int m, int *lbl, int *invlbl) {
  
  void up_sort ( int , int** ) ; 
  
  int i, *b;

  b = calloc(INMAX, sizeof(int));

  for ( i=0 ; i<m ; i++ ) b[i] = lbl[(*a)[i]] ; 

  /* sort b from lowest to highest */ 
  
  up_sort ( m, &b ) ;   

  for ( i=0 ; i<m ; i++ ) (*a)[i] = invlbl[ b[i] ] ; 

  free(b) ; 

}
  

/***********************************************************************/
/*       Sort integers from lowest to highest                          */

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


/***********************************************************************/
/*        Invert a node labeling                                       */

void invert ( int *a, int *b, int k ) { 

  int i ; 
  
  for ( i=0 ; i<k ; i++ ) b[ a[i] ] = i ; 

} 






