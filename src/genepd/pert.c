/* pert.c */
/* data structure and functions to support perturbation matrix */
/* with multiple perturbations in each condition */
/* Trey Ideker March 1999 */

#include "pert.h"

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
  value = c - '0';  /* convert character to int */
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

void printPert (pertMatrixType MatIn, int num_conds, int num_nodes) {

  int row, col;

  for (row=0; row < num_conds; row++) {
    for (col = 0; col < num_nodes; col++) {
      fprintf(stderr, "%c ", MatIn[row][col]);
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");

}
