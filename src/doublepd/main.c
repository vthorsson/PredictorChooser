/* DOUBLEPD- choosing between several hypothetical network   */
/* models using double perturbation experiments.  Doublepd   */
/* determines which double perturbation experiments can      */
/* discriminate between a given list of network models       */
/*                                                           */
/* May 1999 */
/* Copyright (c) Trey Ideker and Vesteinn Thorsson */

/* Options: -d        = debug mode                           */
/*          -o <file> = output max. entropy pert to file     */
/*                                                           */
/*************************************************************/

#include <stdlib.h>
#include <math.h>
#include "graph_tools.h"
#include "bit.h"
#include "entropy.h"

int main ( int argc, char *argv[] ){

  int a, b, dir;
  int i, j, count, retval, numfiles;
  int N, M;
  int ***intransALL, ***intrans_upALL, **state; 
  BOOL debug = FALSE;
  char **files, *pertOutFile, aDir, bDir;
  entropy *H;
  network *net ; 
  FILE *fp;

  /* seed random number generator */
  srand(time(NULL));

  /* parse command line, check to see if debug flag  */
  files = calloc(argc, sizeof(char *));
  files[0] = NULL;
  pertOutFile = NULL;
  for (i=1, numfiles=0; i<argc; i++) {
    if (strcmp(argv[i], "-d") == 0) debug = TRUE ;
    else if (strcmp(argv[i], "-o") == 0) pertOutFile = argv[++i];
    else files[numfiles++] = argv[i] ;
  }
  if (files[0] == NULL || files[0] == "") {
    printf("USAGE: doublepd [-d] [-o <file>] <net1> <net2> ... <netM>\n");
    printf("doublepd v. %s\n", __DATE__); 
    exit(0);
  }; 

  /* allocate memory */
  net = calloc(numfiles, sizeof(network));
  intransALL = calloc(numfiles, sizeof(int **)); 
  intrans_upALL = calloc(numfiles, sizeof(int **)); 

  /* Read in all network models specified */
  printf("READING IN ALL NETWORK HYPOTHESES\n");
  for (i = 0, j = 0; i < numfiles ; i++) {
    /* read in one network */
    net[j] = modelf ( files[i], debug);
    if (i == 3) print_model (&net[j], stdout) ;
    /* convert network to upper_triangular */
    intrans_get ( &intransALL[j], net[j] );     
    retval = Upper_triangular ( &net[j], intransALL[j] ) ;
    /* only continue with this network if UT form found */
    if (retval == 0) {
      intrans_get ( &intrans_upALL[j], net[j] ) ;
      j++;
    }
    else printf("Skipping file %s: upper triangular form not found\n",
		files[i]);
  }

  /* N is the number of nodes, M the number of networks */
  N = net[0].N ; 
  M = j ;
  printf("N = %d NODES; M = %d NETWORKS\n\n", N, M);

  /* initialize state vectors */
  state = calloc(M, sizeof(int *));  /* holds N node values for M networks */ 
  for (i = 0; i < M; i++) state[i] = calloc(N, sizeof(int));
  H = calloc(4*N*N, sizeof(entropy)); /* allocate for all double perts */

  /* iterate over all double perturbations to network (a,b) */
  for (a = 0, count = 0; a < N; a++) {
    for (b = a+1; b < N; b++) {
      /* iterate over all 4 directions of forcing a and b (up / down) */
      for (dir = 0; dir < 4; dir++) {
	/* add proper direction to a and b, put in aDir and bDir */
	aDir = bDir = '-';
	if (dir == 2 || dir == 3) aDir = '+';
	if (dir == 1 || dir == 3) bDir = '+';
       	/* iterate over all networks i for this perturbation (a,b) */
	for (i = 0; i < M; i++) {
	  /* get state for net i under perts a and b */
	  getStateUT(a, b, aDir, bDir, net[i], intrans_upALL[i], state[i]);
	  /* if debug mode, print diagnostic output */
	  if (debug == TRUE) {
	    printf("Pert (%d%c, %d%c) Network %d: ", a, aDir, b, bDir, i);
	    for (j=0; j<N; j++) printf("%d ", state[i][j]);
	    printf("\n");
	  }
	}
	/* determine frequency of each state for this perturbation, and */
	/* determine entropy for this perturbation given frequencies */

	H[count].pert[0] = a;
	H[count].pert[1] = b;
	H[count].dir[0] = aDir;
	H[count].dir[1] = bDir; 

	getEntropy(state, M, N, &H[count], FALSE); 
	if (debug == TRUE) printf("Entropy = %5.2f\n\n", H[count].value);
	if (H[count].value > 0.0) {
	  count++; /* increment for non-0 Hvalue */
	}
      } /* end dir loop */
    } /* end pert b loop */
  } /* end pert a loop */

  /* sort and print entropies from highest to lowest */
  qsort(H, count, sizeof(entropy), entropySort);
  printf("MAX ENTROPY FOUND: %7.2f\n", H[0].value);
  printf("MAX ENTROPY POSSIBLE ON %d NETWORKS = %5.2f\n", 
	 M, log(M) / log(2) );
  printf("NON-ZERO ENTROPIES: %d\n\n", count);
  printf("Pert1 Pert2 #states Entropy\n");
  printf("----- ----- ------- -------\n");
  for (i=0; i<count; i++) printf("%4d%c %4d%c %7d %7.2f\n", 
				 H[i].pert[0], H[i].dir[0],
				 H[i].pert[1], H[i].dir[1],
				 H[i].states, H[i].value);
  printf("\n");

  /* print freq distribution of max entropy */
  printf("STATE DISTRIBUTION OF MAX ENTROPY PERTURBATION (count, state):\n"); 
  for (i=0; i<M; i++) getStateUT(H[0].pert[0], H[0].pert[1], H[0].dir[0], 
				 H[0].dir[1], net[i], intrans_upALL[i], 
				 state[i]);  
  getEntropy(state, M, N, &H[0], TRUE); 
  printf("\n");

  /* if requested, output max entropy pert to file */
  if (pertOutFile != NULL) {
    fp = fopen(pertOutFile, "w");
    if (fp == NULL) exit(1);
    fprintf(fp, "%d%c %d%c\n", H[0].pert[0], H[0].dir[0], 
	    H[0].pert[1], H[0].dir[1]);
    fclose(fp);
  }

  /* clear memory */
  for (i=0;i<M;i++) {
    delete_model(&net[i]);
  }
  free(net);

  for (i=0;i<M;i++) {
    free(state[i]);
  }
  free(state);

  for (i=0; i<M; i++) {
    for (j=0; j<N; j++){
      free(intransALL[i][j]);
      free(intrans_upALL[i][j]);
    }
    free(intransALL[i]); 
    free(intrans_upALL[i]);
  }
  free(intransALL);
  free(intrans_upALL);

  free(H);
  free(files);
  return(0);
}



