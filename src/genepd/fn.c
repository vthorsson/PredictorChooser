/* FN.C - determines final network graph and functions from */
/* a provided set of simplest output solutions              */
/* Trey Ideker 2/10/99                                      */

#include "fn.h"

/*****************************************************************/
/* computes adjaceny matrix of predicted graph "pred", and assoc.*/
/* functions "fn".  Inputs set stack, perturbation matrix,       */
/* number of variables, and file pointer */

void MinFn(stack **sets, pertMatrixType pert, 
	   int num_conds, int num_vars, char *outfile_base) {
  
  char **fn;                        /* output functions */
  int **pred;                       /* output prediction matrix */
  int row, col; 
  BitVectorType inputs;
  FILE *fp;
  
  /* dynamically initialize memory */
  if (NewBitVector(&inputs, num_vars) == FALSE) die(0);
  fn = calloc(num_vars, sizeof(char *));
  pred = calloc(num_vars, sizeof(int *));
  if (fn == NULL || pred == NULL) die(0);
  for (row=0; row<num_vars; row++) {
    pred[row] = calloc(num_vars, sizeof(int));
    if (pred[row] == NULL) die(0);
    for (col=0; col<num_vars; col++) {
      pred[row][col] = 0;
    }
  }

  /* step through all variables */
  for (col = 0; col < num_vars; col++) {

    /* retrieve solution from stack and output to screen and */
    /* to prediction (adjacency) matrix */
    ClearBitVector(&inputs);
    
    if (empty_stack(sets[col]) == FALSE)
      CopyBitVector(top_of_stack(sets[col]), &inputs);
    if (debug == TRUE) fprintf(stderr, "[%d] depends on: ", col);

    /* using inputs and pert, determine for this col the matrices */
    /* pred and fn */
    addSoln(inputs, pert, pred, fn, col, num_conds, num_vars);

  } /* end col for loop */
  
  fp = fopen(outfile_base, "w");
  SaveOutput(fp, num_vars, pred, fn);
  fclose(fp); 

  /* free memory */
  for (col = 0; col < num_vars; col++) {
    free(fn[col]);
    free(pred[col]);
  }
  free(fn);
  free(pred);
  KillBitVector(&inputs);
}

/*****************************************************************/
/* like MinFn and MinFnAll, but randomly choses up to limit_num  */
/* number of solutions.                                          */
/* added by Trey Ideker on 6/3/99                                */

void MinFnRand(stack **sets, pertMatrixType pert, 
	   int num_conds, int num_vars, char *outfile_base) {
  
  char **fn;                        /* output functions */
  int **pred;                       /* output prediction matrix */
  int row, col, soln, num_solns, rnum, i;
  int *outfile_num;
  BitVectorType inputs;
  FILE *fp;
  stack_entry *stack_ptr;

  /* dynamically initialize memory and seed rand num generator */
  outfile_num = malloc(sizeof(int));
  if (outfile_num == NULL) die(0);
  *outfile_num = 0;
  if (NewBitVector(&inputs, num_vars) == FALSE) die(0);
  fn = calloc(num_vars, sizeof(char *));
  pred = calloc(num_vars, sizeof(int *));
  if (fn == NULL || pred == NULL) die(0);
  for (row=0; row<num_vars; row++) {
    pred[row] = calloc(num_vars, sizeof(int));
    if (pred[row] == NULL) die(0);
    for (col=0; col<num_vars; col++) pred[row][col] = 0;
  }
  srand(time(NULL));

  /* perform number of iterations specified by limit */
  for (soln = 0; soln < rand_num; soln++) {
    /* step through all variables */
    for (col = 0; col < num_vars; col++) {
      
      /* retrieve solution from stack and output to screen and */
      /* to prediction (adjacency) matrix */
      ClearBitVector(&inputs);
      
      /* determine number of solutions in stack */ 
      num_solns = stack_depth(sets[col]);
      
      /*randomly choose a soln using rnum ranging between 0 and num_solns */
      if (empty_stack(sets[col]) == FALSE) {
	rnum = floor( num_solns * ( (double) rand() / (double) RAND_MAX ) );
	get_entry(sets[col], rnum, &inputs);
      }
      if (debug == TRUE) 
	fprintf(stderr, "[%d] depends on (soln %d): ", col, rnum);
      
      /* using inputs and pert, determine for this col the matrices */
      /* pred and fn */
      addSoln(inputs, pert, pred, fn, col, num_conds, num_vars);
      
    } /* end col for loop */  

    /* write this soln to file */
    if (debug == TRUE) fprintf(stderr, "Writing solution %d\n", *outfile_num);
    SaveOutputAll(outfile_base, *outfile_num, num_vars, pred, fn);
    *outfile_num = *outfile_num + 1;

  } /* end soln for loop */

  /* free memory */
  for (col = 0; col < num_vars; col++) {
    free(fn[col]);
    free(pred[col]);
  }
  free(fn);
  free(pred);
  KillBitVector(&inputs);
  free(outfile_num);
}


/*****************************************************************/
/* computes adjaceny matrix of predicted graph "pred", and assoc.*/
/* functions "fn".  Inputs set stack, perturbation matrix,       */
/* number of variables, and file pointer */

void MinFnAll(stack **sets, pertMatrixType pert, 
	      int num_conds, int num_vars, char *outfile_base) {
  

  char **fn;                        /* output functions */
  int **pred;                       /* output prediction matrix */
  int i, row, col;
  int *outfile_num;                 /* tracks file output number */

  /* dynamically initialize memory */
  outfile_num = malloc(sizeof(int));
  fn = calloc(num_vars, sizeof(char *));
  pred = calloc(num_vars, sizeof(int *));  
  if (fn == NULL || pred == NULL || outfile_num == NULL) die(0);
  for (row=0; row<num_vars; row++) {
    pred[row] = calloc(num_vars, sizeof(int));
    fn[row] = NULL;
    if (pred[row] == NULL) die(0);
    for (col=0; col<num_vars; col++) {
      pred[row][col] = 0;
    }
  }
  *outfile_num = 0;

  /* launch recursion with variable 0 and outfile_num 0*/
  build_net(sets, pert, pred, fn, 0, num_conds, num_vars,
	    outfile_base, outfile_num);

  /* free memory */
  for (i=0; i<num_vars; i++) {
    free(pred[i]);
    free(fn[i]);
  }
  free(pred);
  free(fn);
  free(outfile_num);
}

/*************************************************************/

void build_net(stack **sets, pertMatrixType pert, int **pred, 
	       char **fn, int var, int num_conds, int num_vars,
	       char *outfile_base, int *outfile_num) {

  BitVectorType *inputs=NULL, *store=NULL;
  BOOL retval;

  if (debug == TRUE)
    fprintf(stderr, "Performing recursion w/ var %d\n", var);

  /* initialize memory */
  inputs = malloc(sizeof(BitVectorType));
  store = malloc(sizeof(BitVectorType));
  if (NewBitVector(inputs, num_vars) == FALSE) die(0);
  if (NewBitVector(store, num_vars) == FALSE) die(0);

  /* check to see if current net soln contains all variables. If so, */
  /* output prediction information to file with correct iteration    */
  /* number appended to the end */
  if (var == num_vars) {
    if (debug == TRUE) fprintf(stderr, "Writing solution %d\n", *outfile_num);
    SaveOutputAll(outfile_base, *outfile_num, num_vars, pred, fn);
    *outfile_num = *outfile_num + 1;
  }

  /* otherwise, get next solution off stack, add it to pred and */
  /* fn matrices, and recurse on next variable*/
  else {
    if (debug == TRUE)
      fprintf(stderr, "Adding inputs for var %d to matrix: ", var);
    if (empty_stack(sets[var]) == FALSE) 
      CopyBitVector(top_of_stack(sets[var]), inputs);
    addSoln(*inputs, pert, pred, fn, var, num_conds, num_vars);
    build_net(sets, pert, pred, fn, var+1, num_conds, num_vars, 
	      outfile_base, outfile_num);

    /* if possible, recurse w/ next soln in stack for this variable, */
    /* but only if max number of solns (limit_num) not reached */
    pop_stack(sets[var], store);
    if (debug == TRUE) {
	fprintf(stderr, "Popping inputs for var %d: ", var);
	PrintBitVector(*store);
	fprintf(stderr, "\n");
	fprintf(stderr, "Next item on stack = ");
	if ( empty_stack(sets[var]) == FALSE ) 
	    PrintBitVector(top_of_stack(sets[var]));
	fprintf(stderr, "\n");
    }
    if (empty_stack(sets[var]) == FALSE && 
	(limit == FALSE || *outfile_num < limit_num) ) {
      if (debug == TRUE) fprintf(stderr, "Recursing on right sib\n");
      build_net(sets, pert, pred, fn, var, num_conds, num_vars,
		outfile_base, outfile_num);    

    }
    retval = push_stack(sets[var], *store);
    if (debug == TRUE) {
      fprintf(stderr, "Pushed inputs for var %d: ", var);
      PrintBitVector(top_of_stack(sets[var]));
      fprintf(stderr, "\n");
    }
  }  /* end else */

  /* clean up memory */
  KillBitVector(inputs);
  KillBitVector(store);
  free(inputs);
  free(store); 
  inputs = NULL;
  store = NULL;
 if (debug == TRUE) 
    fprintf(stderr, "Finished Recursion w/ var %d\n", var);
} /* end function */

/******************************************************************/
/* function which sets the prediction matrix and finds the fn for */
/* a particular set of inputs to var.                             */

void addSoln (BitVectorType inputs, pertMatrixType pert, int **pred, 
	      char **fn, int var, int num_conds, int num_vars) {
  
  int i, k, size, row2, col2, cond, value;
  unsigned int index;

  /* set prediction matrix values */
  for (i=0; i<num_vars; i++) {
    if (GetBitValue(inputs,i) == 1) {
      if (debug == TRUE) fprintf(stderr, "[%d] ", i);
      pred[i][var] = 1;
    }
    else pred[i][var] = 0;
  }    
  /* set function string to starting state of all dont cares (*).
     size of function is 2 ^ number of sets, which is given by i.  
     Mark the end of the function list by inserting a bar (|) */
  size = pow(2, CountOneBitVector(inputs) );
  if (fn[var] != NULL) free(fn[var]);
  fn[var] = calloc(size+1, sizeof(char));
  if (fn[var] == NULL) die(0);
  for (k=0; k < size; k++) fn[var][k] = '*';
  fn[var][k] = '|';
  
  /* now that we know indep. variables, determine function */
  for (cond=0; cond < num_conds; cond++) {
    /* skip row if its the perturbation due to the dependent variable */
    if (isPert(pert, cond, var) == FALSE) {
      /* calculate index */
      k = 0;
      index = 0;
      for (col2=0; col2 < num_vars; col2++) {
	if ( GetBitValue(inputs, col2) == 1 ) {
	  index = index | (getValue(pert, cond, col2) << k);
	  k++;
	}
      } /* end col2 for loop */
      /* with index calculated, set function */
      value = getValue(pert, cond, var);
      if (value == 1) fn[var][index] = '1';
      else if (value == 0) fn[var][index] = '0';
      else die(5);
    }
  } /* end conditions for loop */
  
  /* print out the final function */
  if (debug == TRUE) {
    fprintf(stderr, "\nFunction: ");
    for (k=size-1; k >= 0; k--) fprintf(stderr, "%c", fn[var][k]);
    fprintf(stderr, "\n\n");
  }
}

/*************************************************************/
/* clear the column corresponding to var and clear var entry */
/* from fn.                                                  */

void clearSoln(int **pred, char **fn, int var, int num_vars) {

 int row;

 for (row = 0; row < num_vars; row++) {
   pred[row][var] = 0;
 }
 free(fn[var]);
}

/************************************************************/
