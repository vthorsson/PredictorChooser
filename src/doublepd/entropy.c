/* Entropy calculation and frequency counting tools */
/* for DOUBLEPD program                             */
/* May 27, 1999 by Trey Ideker                      */

#include "entropy.h"

/*************************************************************/

void getStateUT (int a, int b, char aDir, char bDir, network net, 
		 int **intrans_up, int *state) {

  int j, N;
  int aT, bT, *O, *labelsT;
  BitVectorType storeA, storeB;

  /* initialize */
  N = net.N;
  labelsT = calloc(N, sizeof(int));
  O = calloc(N, sizeof(int));
  if (labelsT == NULL || O == NULL ) exit(1);
  /* transform a and b to their upper-triangular labels */
  invert(net.label, labelsT, N);
  aT = labelsT[a];
  bT = labelsT[b];
  /* store unperturbed network state */
  NewBitVector( &storeA, (net.out[aT]).NoBits ) ; 
  NewBitVector( &storeB, (net.out[bT]).NoBits ) ; 
  CopyBitVector( net.out[aT], &storeA ) ; 
  CopyBitVector( net.out[bT], &storeB ) ; 
  /* impose perturbations on network */
  ClearBitVector( &net.out[aT] ) ; /* by default, force a, b low */
  ClearBitVector( &net.out[bT] ) ; 
  if (aDir == '+') OnesBitVector(&net.out[aT]); 
  if (bDir == '+') OnesBitVector(&net.out[bT]);
  /* obtain state of network */
  get_output (O, net, intrans_up);
  /* retransform nodes to their original labels */
  for (j = 0; j < N; j++) state[ net.label[j] ] = O[j];
  /* restore state of network (no perturbations) */
  CopyBitVector( storeA, &net.out[aT] ) ; 
  CopyBitVector( storeB, &net.out[bT] ) ; 
  KillBitVector( &storeA ) ; 
  KillBitVector( &storeB ) ;
  /* clear memory */
  free(labelsT);
  free(O);
  
}

/*************************************************************/

void getEntropy (int **state, int M, int N, entropy *H, 
		 BOOL print) {
  
  /* state is a 2D array holding M networks by N node values for each */
  /* The N node values for each network is called the state.          */
  /* H holds the entropy information                                  */

  stack *freq;      /* stack holding distinct states and freq of each */
  float temp;
  int i, count;
  BitVectorType stateBV;
  BOOL retval;

  freq = new_stack();  
  if (freq == NULL) exit(1);
  NewBitVector(&stateBV, N);
  H->value = 0.0;         /* ensure entropy starts out at zero */

  /* count distribution of distinct states */
  for (i=0; i<M; i++) {
    /* convert each network state to bit vector */
    IntArray2BitVector(&stateBV, state[i]);
    /* compare state against stack: already_in_stack will add a count of */
    /* one to the entry if it finds it in stack */
    retval = already_in_stack(freq, stateBV);
    if (retval == FALSE) push_stack(freq, stateBV);
  }

  /* calculate entropy H over distribution of all states */
  /* first get number of distinct states L */
  H->states = stack_depth(freq);
  for (i = 0; i < H->states; i++) {
    /* pop stack to get count for next state in stack */
    pop_stack(freq, &stateBV, &count);
    /* print out info */
    if (print == TRUE) {
      printf("%5d,", count);
      PrintBitVector(stateBV);
      printf("\n");
    }
    temp = (float) count / (float) M;
    H->value -= temp * log ( temp ) / log(2);
  }

  /* clean up */
  KillBitVector(&stateBV);
  kill_stack(freq);
}

/******************************************************************/
/* function to be used with qsort for sorting entropy array       */
/* descending order sort                                          */

int entropySort (const void *vp, const void *vq) {
  
  const entropy *p = vp, *q = vq;
  float x;

  x = (float) p->value - (float) q->value;
  return ((x == 0.0) ? 0 : (x < 0.0) ? +1 : -1);
  
}
