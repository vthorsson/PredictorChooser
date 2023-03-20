/* Formatted file IO for gene network prediction program */
/* January 1999 */
/* Copyright (c) Trey Ideker and Vesteinn Thorsson */
/********************************************************/

#include "io.h"

/* ReadPerturbs returns matrix read from file in 2D array matOut */
/* also stores number of variables read in varOut and number of  */
/* conditions read in condOut                                    */

pertMatrixType ReadPerturbs (FILE *fp, int *condsOut, int *varsOut)
{
  int i, j, num_conds=0, num_vars=0;
  char c, s[2];
  pertMatrixType matOut;
  BOOL first;

  /* count number of rows and columns to get size of matrix*/
  /* num of rows = num of conditions, num of cols = number of vars */
  first = TRUE;
  while ((c = getc(fp)) != EOF) {
    if ( (first == TRUE) && 
	 (c == '1' || c == '0' || c == '+' || c == '-') ) num_vars++;
    if (c == '\n') {
      first = FALSE;
      num_conds++;
    }
  }
  *condsOut = num_conds;
  *varsOut = num_vars;

  /* allocate space for new pertMatrixType matrix */
  matOut = calloc (num_conds, sizeof(char *));
  if (matOut == NULL) die(0);
  for(i=0;i<num_conds;i++) {
    matOut[i] = calloc (num_vars, sizeof(char));
    if (matOut[i] == NULL) die(0);
    for (j=0; j<num_vars; j++) {
      putValue(matOut, i, j, '0');
    }
  }

  /* rewind file and read in actual values */
  rewind(fp);
  for(i=0;i<num_conds;i++) {
    for(j=0;j<num_vars;j++) {
      fscanf(fp,"%s", s);
      putValue(matOut, i, j, s[0]);
    }
  }	
  return matOut;
}

/*****************************************************************/

/* SaveOutput writes graph (matIn) and functions (fnIn) to file */
/* along with the number of variables (varsIn) */

void SaveOutput(FILE *outfile, int varsIn, int **matIn, char **fnIn) 
{

  int row, col, k, count;

  fprintf(outfile, "num_nodes %d\n\n", varsIn);
  fprintf(outfile, "Graph\n");
  for (row = 0; row < varsIn; row++) {
    for (col = 0; col < varsIn; col++) {
      fprintf(outfile, "%d ", matIn[row][col]);
    }
    fprintf(outfile, "\n");
  }

  fprintf(outfile, "\nFunctions\n");
  for (col = 0; col < varsIn; col++) {
    count = 0;
    fprintf(outfile, "[%d]: ", col);
    while (fnIn[col][count] != '|') count++;
    for (k = count-1; k >= 0; k--)
      fprintf(outfile, "%c", fnIn[col][k]);
    fprintf(outfile, "\n");
  }

}

/********************************************************/
/* SaveOutput writes graph (matIn) and functions (fnIn) to file */
/* along with the number of variables (varsIn) */

void SaveOutputAll(char *file_base, int file_num, int varsIn, 
		   int **matIn, char **fnIn) {

  int row, col, k, count;
  char num_string[32], temp_string[32];
  FILE *outfile;

  sprintf(num_string, "%d", file_num);
  strcpy(temp_string, file_base);
  strcat(temp_string, num_string);
  outfile = fopen(temp_string, "w");

  fprintf(outfile, "num_nodes %d\n\n", varsIn);
  fprintf(outfile, "Graph\n");
  for (row = 0; row < varsIn; row++) {
    for (col = 0; col < varsIn; col++) {
      fprintf(outfile, "%d ", matIn[row][col]);
    }
    fprintf(outfile, "\n");
  }

  fprintf(outfile, "\nFunctions\n");
  for (col = 0; col < varsIn; col++) {
    count = 0;
    fprintf(outfile, "[%d]: ", col);
    while (fnIn[col][count] != '|') count++;
    for (k = count-1; k >= 0; k--)
      fprintf(outfile, "%c", fnIn[col][k]);
    fprintf(outfile, "\n");
  }

  fclose(outfile); 
}

/*************************************************************/
/* write statistics to file */

void SaveStats (char *infofile, stack **sets, int num_vars) {

  int var, depth;
  double user_time, totalSolns = 1.0;
  FILE *fp;

  fp = fopen(infofile, "w");
  /* record # solns for each variable */
  fprintf(fp, "NUMBER OF SOLUTIONS\n");
  fprintf(fp, "var  solns\n");
  for (var = 0; var < num_vars; var++) {
    depth = stack_depth(sets[var]);
    totalSolns *= depth;
    fprintf(fp, "%3d  %5d\n", var, depth);
  }
  fprintf(fp, "total_solns %5.2e\n", totalSolns);

  /* record total elapsed time of inference */
  fprintf(fp, "\nELAPSED TIME\n");
  prn_time(fp);
  fclose(fp);

}
