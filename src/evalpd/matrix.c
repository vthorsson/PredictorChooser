/* Matrix manipulation and other utilities */
/* for gene network prediction program */
/* January 1999 */
/* Copyright (c) Trey Ideker and Vesteinn Thorsson */

#include "matrix.h"

/****************************************************************/

int mat_prod(int num_vars, int **mat1, int **mat2, int **matOut) {

  /* returns 1 if output matrix contains any non-zero elements */

  int row, col, i, output;
  int **temp;
  output = 0;

  temp = calloc(num_vars, sizeof(int *));
  for (row=0; row < num_vars; row++) {
    temp[row] = calloc(num_vars, sizeof(int));
    for (col=0; col < num_vars; col++) {
      temp[row][col] = 0;
      for (i = 0; i < num_vars; i++) {
	temp[row][col] += mat1[row][i] * mat2[i][col]; 
      }
      if (temp[row][col] != 0) output = 1;
    }
  } 

  /* copy temp to matOut */
  /* must use temp matrix in case matOut is same matrix */
  /* mat1 or mat2 */
  for (row=0; row < num_vars; row++) {
    for (col=0; col < num_vars; col++) {
      matOut[row][col] = temp[row][col];
    }
  }

  /* free temp */
  for (i=0; i<num_vars;i++) free(temp[i]);
  
  return output;

}

/*****************************************************************/

int mat_or(int num_vars, int **mat1, int **mat2, int **matOut) {

  /* returns number of nonzero elements in matrix */

  int row, col, i, output;
  int **temp;
  int num_elem = 0;

  temp = calloc(num_vars, sizeof(int *));
  for (row=0; row < num_vars; row++) {
    temp[row] = calloc(num_vars, sizeof(int));
    for (col=0; col < num_vars; col++) {
	temp[row][col] = (mat1[row][col] || mat2[row][col]);
	if (temp[row][col] != 0) num_elem++;
    }
  } 

  /* copy temp to matOut */
  /* must use temp matrix in case matOut is same matrix */
  /* as mat1 or mat2 */
  for (row=0; row < num_vars; row++) {
    for (col=0; col < num_vars; col++) {
      matOut[row][col] = temp[row][col];
    }
  }

  /* free temp */
  for (i=0; i<num_vars;i++) free(temp[i]);

  return num_elem;

}

/*****************************************************************/

int mat_and(int num_vars, int **mat1, int **mat2, int **matOut) {

  /* returns number of nonzero elements in matrix */

  int row, col, i, output;
  int **temp;
  int num_elem = 0;

  temp = calloc(num_vars, sizeof(int *));
  for (row=0; row < num_vars; row++) {
    temp[row] = calloc(num_vars, sizeof(int));
    for (col=0; col < num_vars; col++) {
	temp[row][col] = (mat1[row][col] && mat2[row][col]);
	if (temp[row][col] != 0) num_elem++;
    }
  } 

  /* copy temp to matOut */
  /* must use temp matrix in case matOut is same matrix */
  /* as mat1 or mat2 */
  for (row=0; row < num_vars; row++) {
    for (col=0; col < num_vars; col++) {
      matOut[row][col] = temp[row][col];
    }
  }

  /* free temp */
  for (i=0; i<num_vars;i++) free(temp[i]);
  free(temp);
  return num_elem;

}

/****************************************************************/
/* compute transitive closure of a directed acyclic graph       */

int ** FindClosure(int **matIn, int num_vars) {

  int row, col, i, flag;
  int **temp, **matOut;

  /* allocate space for temp and output (closure) matrix and copy */
  /* input matrix (dag) to temp */
  matOut = calloc (num_vars, sizeof(int *));
  temp = calloc (num_vars, sizeof(int *));
  if (matOut == NULL || temp == NULL) die(0);
  for (row=0; row < num_vars; row++) {
    matOut[row] = calloc (num_vars, sizeof(int));
    temp[row] = calloc (num_vars, sizeof(int));
    for (col = 0; col < num_vars; col++) {
      matOut[row][col] = matIn[row][col];
      temp[row][col] = matIn[row][col];
    }
  }

  /* n_th exponent of DAG gives all paths of length n. */
  /* To find all paths of any length, compute exponent for */
  /* increasing values of n until no paths found, i.e. */
  /* exponent (flag) = 0 */
  do {
    flag = mat_prod(num_vars, matIn, temp, temp);
    mat_or(num_vars, matOut, temp, matOut);
  }
  while (flag == 1);

  /* free temp */
  for (i=0; i<num_vars;i++) free(temp[i]);
  return matOut;
}

/*****************************************************************/
/* spools a matrix to standard error output                      */
/*****************************************************************/

void printMatrix (int **MatIn, int num_rows, int num_cols) {

  int row, col;

  for (row=0; row < num_rows; row++) {
    for (col = 0; col < num_cols; col++) {
      fprintf(stderr, "%d ", MatIn[row][col]);
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");

}


