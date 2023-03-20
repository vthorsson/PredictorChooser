/* pert.c */
/* data structure and functions to support perturbation matrix */
/* with multiple perturbations in each condition */
/* Trey Ideker March 1999 */

#include "pert.h"

/************************************************************/
/* initialize pert matrix                                   */

pertMatrixType newPert(int num_conds, int num_nodes) {
  
  int i, j;
  pertMatrixType mat;

  mat = calloc(num_conds, sizeof(char *) );
  if (mat == NULL) exit(1);
  for(i=0;i<num_conds;i++) {
    mat[i] = calloc (num_nodes, sizeof(char));
    if (mat[i] == NULL) exit(1);
    for (j=0; j<num_nodes; j++) putValue(mat, i, j, '0');
  }
  return mat;
}

/************************************************************/
/* free memory for pert matrix                              */

void freePert(pertMatrixType mat, int num_conds) {
  
  int i;

  for(i=0;i<num_conds;i++) free(mat[i]);
  free(mat);

}

/********************************************************************/
/* store a value in the matrix at a particular location             */
/* i.e. the value of a particular node under a particular condition */

void putValue(pertMatrixType mat, int cond, int node, char value) {

  mat[cond][node] = value;

}

/********************************************************************/
/* retreive a value from the matrix at a particular location             */
/* i.e. the value of a particular node under a particular condition */

int getValue(pertMatrixType mat, int cond, int node) {

  int value;
  char c;
  
  c = mat[cond][node];
  /* convert + or - to 1 or 0, respectively */
  if (c == '+') c = '1';
  else if (c == '-') c = '0';
  value = c - '0';
  return value;
}

/*********************************************************************/
/* query a particular node and condition to see if it is a perturbed */
/* returns true if a perturbation, false otherwise                   */

BOOL isPert(pertMatrixType mat, int cond, int node) {

  BOOL retval = FALSE;

  if (mat[cond][node] == '+' || mat[cond][node] == '-')
    retval = TRUE;

  return retval;

}


/*****************************************************************/
/* spools a pert matrix to standard error output                      */
/*****************************************************************/

void printPert (pertMatrixType MatIn, int num_conds, int num_nodes,
		FILE *fp) {

  int row, col;

  for (row=0; row < num_conds; row++) {
    for (col = 0; col < num_nodes; col++) {
      fprintf(fp, "%c ", MatIn[row][col]);
    }
    fprintf(fp, "\n");
  }
}


/**************************************************************/
/* added by Trey Ideker May 1999                              */
/* reads list of perturbations from a file                    */

pertMatrixType getPertList ( FILE *fp, int N, int *M ) {

  int cond, pert = 0, *num_perts, node;
  int num_nodes = N, num_conds;
  char c, dir;
  pertMatrixType mat;

  /* get num of conditions by counting lines in file */
  /* start with num_perts = 1 b/c every row has at least one pert */
  /* start with num_conds = 1 b/c cond 0 is assumed to be wt */
  num_conds = 1;
  while ((c = getc(fp)) != EOF) if (c == '\n') num_conds++;
  num_perts = calloc(num_conds, sizeof(int *) );
  for (cond = 0; cond < num_conds; cond++) num_perts[cond] = 1;
  *M = num_conds;

  /* go back through file to get number of perts for each cond */
  rewind(fp);
  cond = 1;
  while ((c = getc(fp)) != EOF) {
    if (c == '\n') cond++;
    if (c == ' ') num_perts[cond]++;
  }

  /* allocate space for pert matrix  */
  /* by default, make first row wt */
  mat = newPert(num_conds, num_nodes);

  /* go back through a third time, reading in actual values */
  rewind(fp);
  for (cond = 1; cond < num_conds; cond++) {    
    for (pert = 0; pert < num_perts[cond]; pert++) {
      fscanf(fp, "%d%c", &node, &dir);
      putValue(mat, cond, node, dir);
    }
  }
  return mat;
}

/**************************************************************/
/* added by Trey Ideker June 1999                             */
/* generates list of all single perturbations                 */
/* and writes to a file                                       */

pertMatrixType singlePertList ( FILE *fp, int N, 
				network net, int **intrans_up) {

  int node, cond, *state, *stateT;
  int num_nodes, num_conds;
  char c, dir;
  pertMatrixType mat;

  /* initialize vars and allocate space for pert matrix  */
  num_nodes = N;
  num_conds = N+1;
  state = calloc (N, sizeof(int));
  stateT = calloc (N, sizeof(int));
  mat = newPert(num_conds, num_nodes);

  /* generate wt condition and invert nodes to non-UT */
  get_output (stateT, net, intrans_up);
  for (node = 0; node < N; node++) state[ net.label[node] ] = stateT[node];

  /* generate all single perts */
  for (cond = 1; cond <= N; cond++) {
    node = cond-1;  /* perturb node 0 at row 1 of matrix, and so on */
    if (state[node] == 0) dir = '+';
    else dir = '-';
    putValue(mat, cond, node, dir);
    fprintf(fp, "%d%c\n", node, dir);  /* write to file */
  }

  /* clean up and return pert matrix */
  free(state);
  free(stateT);
  return mat;
}

/********************************************************************/
