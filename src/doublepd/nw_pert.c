/*************************************************************/
/* nw_pert [-1] [-d] <network file> <pert file>              */
/*                                                           */
/*               Generates pert matrix given a network       */
/*               and a list of perturbation conditions.      */
/*               W/ option -1, generates all single          */
/*               perts (away from wt) and stores these       */
/*               in <pert file>. Option -d means debug.      */
/*                                                           */
/* June 1, 1999 by Trey Ideker                               */
/*                                                           */
/* based on nw_del by Vesteinn Thorsson                      */
/*************************************************************/

#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include <string.h>
#include "graph_tools.h"
#include "pert.h"
#include "util.h"

int main ( int argc, char *argv[] ) {

  int j, N, M, **intrans, **intrans_up;   
  int cond, node, nodeT;
  int *state, *stateT, *labelsT;
  network net, net_orig; 
  FILE *fp;
  pertMatrixType pert;
  BitVectorType *storeFn;
  char *infile, *pertfile, usage[80], intChar[2];
  BOOL debug, single;

  /* parse command line, check to see if debug flag  */
  sprintf(usage, "USAGE: nw_pert [-1] [-d] <network file> <pert file>");
  infile = NULL;
  pertfile = NULL;
  debug = FALSE;
  single = FALSE;
  for (j = 1; j < argc; j++) {
    if ( strcmp(argv[j], "-d") == 0 ) debug = TRUE ;
    else if ( strcmp(argv[j], "-1") == 0 ) single = TRUE ;
    else if (infile == NULL) infile = argv[j];
    else if (pertfile == NULL) pertfile = argv[j];
    else {
      fprintf(stderr, "%s\nnw_pert v. %s %s\n\n", usage, __DATE__, __TIME__); 
      return(1);
    }
  }
  if ( (infile == NULL) || (pertfile == NULL) ) {
    fprintf(stderr, "%s\nnw_pert v. %s %s\n\n", usage, __DATE__, __TIME__); 
    return(1);
  }; 

  /* Read in the model and convert to upper triangular form */
  net = modelf ( infile, debug);  
  if (debug == TRUE) {
    fprintf(stderr, "READ IN NETWORK FILE\n");
    print_model(&net, stderr);
  }
  N = net.N ; 
  intrans_get ( &intrans, net );     
  Upper_triangular ( &net, intrans );
  intrans_get ( &intrans_up, net);
  labelsT = calloc(N, sizeof(int));  /* holds the inverted labels  */
  invert(net.label, labelsT, N);

  /* print diagnostic output */
  if (debug == TRUE) {
    fprintf(stderr, "\nCONVERTED TO UPPER TRIANGULAR FORM\n");
    print_model(&net, stderr);
    fprintf(stderr, "\nlabels          =    ");
    for (j=0; j<N; j++) fprintf(stderr, "%d ", net.label[j]);
    fprintf(stderr, "\ninverted labels =    ");
    for (j=0; j<N; j++) fprintf(stderr, "%d ", labelsT[j]);
    fprintf(stderr, "\n\n");
  }
  
  /* if -1 flag, generate all single perturbations */
  if (single == TRUE) {
    /* printf("Writing single perturbations to file %s\n", pertfile); */
    M = N+1;  /* set number of conditions M */
    fp = fopen(pertfile, "w");
    pert = singlePertList (fp, N, net, intrans_up);
    fclose(fp);
  }
  /* Otherwise, default is to read in the perturbations */
  else {
    /* printf("Reading perturbations from file %s\n", pertfile); */
    fp = fopen(pertfile, "r");
    pert = getPertList (fp, N, &M);
    fclose(fp);
  }
  if (debug == TRUE) {
    fprintf(stderr, "GENERATING PERTURBATIONS\n");
    printPert(pert, M, N, stderr);
    fprintf(stderr, "\nOUTPUT STATES:\n");
  }

  /* store all wild type functions: these are all wrt. transformed nodes */
  state = calloc(N, sizeof(int));  /* holds N node values for M networks */
  stateT = calloc(N, sizeof(int));  /* state in transformed node coords  */
  storeFn = calloc(N, sizeof(BitVectorType));
  for (nodeT = 0; nodeT < N; nodeT++) {
    NewBitVector( &storeFn[nodeT], (net.out[nodeT]).NoBits );
    CopyBitVector( net.out[nodeT], &storeFn[nodeT] );
  }

  for (cond = 0; cond < M; cond++) {
    /* set the state of the network for this condition */
    for (node = 0; node < N; node++)
      if ( isPert(pert, cond, node) == TRUE) {
	/* transform this node to UT node */
	nodeT = labelsT[node];
	/* impose pert on network */
	if ( getValue(pert, cond, node) == 0 ) {
	  ClearBitVector( &net.out[nodeT] ) ;
	}
	else {
	  OnesBitVector( &net.out[nodeT] );
	}
      }

    /* obtain new state of network */
    get_output (stateT, net, intrans_up);
    if (debug == TRUE) {
      for (j=0; j<N; j++) fprintf(stderr, "%d ", stateT[ labelsT[j] ] ); 
      fprintf(stderr, "\n");
    } 

    /* Insert value in pert matrix and restore state of network */
    for (node = 0; node < N; node++) {
      nodeT = labelsT[node];
      sprintf(intChar, "%d", stateT[nodeT]);
      if ( isPert(pert, cond, node) == FALSE) 
	putValue(pert, cond, node, intChar[0]);
      else CopyBitVector( storeFn[nodeT], &net.out[nodeT] );
    }
  } /* end for loop stepping through conditions */

  /* output perturbation matrix to stdout */
  printPert(pert, M, N, stdout);

  /* clear memory */
  delete_model(&net);
  freePert(pert, M);

  for (j=0; j<N; j++) { 
    free(intrans[j]); 
    free(intrans_up[j]);
  }
  free(intrans);
  free(intrans_up);

  free(state);
  free(stateT);
  free(labelsT);
  for (j = 0; j < N; j++) KillBitVector( &storeFn[j] );
  free(storeFn);
  return(0);
}









