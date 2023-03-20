/* Program GenePredict    January 1999 */
/* Copyright (c) Trey Ideker and Vesteinn Thorsson */

#include "main.h"

/************************************************************/
/* converts input perturbation matrix into directed         */
/* acyclic graph, represented by an output adjaceny matrix  */

int ** Perturb2DAG(pertMatrixType matIn, int num_conds, int num_vars) {
  
  int row, row2, pertVar, col;
  int diff, thisCond, wildtype;
  int **matOut;
  int pertVal, pertVal2;
  BOOL retval, retval2;

  /* allocate memory for DAG and initialize */
  matOut = calloc (num_vars, sizeof(int *));
  if (matOut == NULL) die(0);
  for (row=0; row < num_vars; row++) {
    matOut[row] = calloc(num_vars, sizeof(int));
    for (col=0; col < num_vars; col++)
      matOut[row][col] = 0;
  }
  
  /* step through all pairs of rows looking for rows with single */
  /* changes in perturbations, e.g. wt and 0- or 0- and 0-1+ */
  for (row = 0; row < num_conds; row++) {
    for (row2 = row+1; row2 < num_conds; row2++) {
      /* find pairs with only one difference in perturbation */
      /* store that difference in pertVar */
      diff = 0;
      for (col = 0; col < num_vars; col++) {
	retval = isPert(matIn, row, col);
	retval2 = isPert(matIn, row2, col);
	if (retval != retval2) {
	  diff++;
	  pertVar = col;
	}
	else if (retval == TRUE) {  
	  /* retval2 is also true in this case */
	  pertVal = getValue(matIn, row, col);
	  pertVal2 = getValue(matIn, row2, col);
	  if (pertVal != pertVal2) {
	    diff++;
	    pertVar = col;
	  }
	}
      }
      /* proceed only if difference is exactly one perturbation */
      if (diff == 1) {
	if (debug == TRUE) 
	  fprintf(stderr, "Var %d: sole perturbation b/w conds %d and %d\n",
		  pertVar, row, row2);
	/* find other vars which are also change between these conds */
	for (col = 0; col < num_vars; col++) {
	  pertVal = getValue(matIn, row, col);
	  pertVal2 = getValue(matIn, row2, col);
	  if ( col != pertVar && pertVal != pertVal2 )
	    matOut[pertVar][col] = 1;
	}
      } /* end if (diff == 1) */
    } /* end for (row2) */
  } /* end for (row) */
  return matOut;
} /* end function */

/*****************************************************************/
/* compute transitive closure of a directed acyclic graph        */
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
  free(temp);

  return matOut;
}

/*****************************************************************/
/* inputs the perturbation matrix and the transitive closure     */
/* returns for each variable a stack with alternate input        */
/* variable combinations */

stack ** MinInputs (pertMatrixType pert, int **closure, 
		    int num_conds, int num_vars) {

  int row, col, row2, col2, numSets, k;
  int match;
  BitVectorType *inequal, *temp, *prev;
  BOOL retval;
  stack_entry *stack_ptr;
  stack *temp_solns;
  stack **sets;
  /* also uses global variable "sets" */

  fprintf(stderr, "Calculating minimum inputs for all variables\n");

  /* dynamically allocate memory for variables */
  inequal = malloc(sizeof(BitVectorType));
  temp = malloc(sizeof(BitVectorType));
  prev = malloc(sizeof(BitVectorType));
  sets = calloc(num_vars, sizeof(stack *));
  if (inequal == NULL || temp == NULL || prev == NULL || sets == NULL) 
    die (1);
  if (NewBitVector (temp, num_vars) == FALSE) die (1);
  if (NewBitVector (prev, num_vars) == FALSE) die (1);
  if (NewBitVector(inequal, num_vars) == FALSE) die (1);

  /* step through each variable in turn to determine its inputs */
  for (col = 0; col < num_vars; col++) {
    /* reset set pointer and initialize stack */
    numSets = 0;
    sets[col] = new_stack();
    if (debug == TRUE) 
      fprintf(stderr, "Building input sets for variable %d:\n", col);
    /* step through possible pairings among rows */
    for (row = 0; row < num_conds; row++) {
      for (row2 = row+1; row2 < num_conds; row2++) {
	/* search for rows which differ in logic value, but
	   skip row if it is the perturbation of the var. in question */
	if ( (isPert(pert, row, col) == FALSE )   &&
	     (isPert(pert, row2, col) == FALSE )  &&
	     (getValue(pert, row, col) != getValue(pert, row2, col) ) ) {
	  /* when found, search all columns in that row for a 
	     corresponding change in logic value */
	  for (col2 = 0; col2 < num_vars; col2++) {
	    /* consider a column (variable) only if it is not reachable
	       in the transitive closure matrix */
	    if ( (getValue(pert, row, col2) != getValue(pert, row2, col2)) &&
		 (col != col2) && 
		 (closure[col][col2] == 0) ) 
	      /* put a one at the correct bit */
	      retval = BitVectorIns (inequal, col2);
	    /* to put a zero at the correct bit, do nothing */	    
	  }
	  if (debug == TRUE) {
	    fprintf(stderr, "   %d: (%d) + (%d): ", numSets, row, row2);
	    PrintBitVector(*inequal);
	    fprintf(stderr, "\n");
	  }

	  /* done calculating one set of possible inputs */
	  /* now determine if this set is already in stack */
	  stack_ptr = sets[col]->top;
	  k = 0;
	  match = 0;
	  while (!match && k < numSets) {
	    CopyBitVector(stack_ptr->data, prev);
	    retval = BitVectorAND(*inequal, *prev, temp);
	    if (BitVectorEQ(*temp, *inequal) == TRUE) {
	      /* most recent row a subset of a previous row in the stack */
	      /* replace previous row with recent one and end while loop */
	      match = 1;
	      CopyBitVector(*temp, prev);
	      change_entry(stack_ptr, *prev);
	      if (debug == TRUE) 
		fprintf(stderr, "   Subset of %d\n", numSets-1-k);
	    }
	    else if (BitVectorEQ(*temp, *prev) == TRUE) {
	      /* most recent row a superset of a row in the stack */
	      /* don't change stack but end while loop */
	      match = 1;
	      if (debug == TRUE) 
		fprintf(stderr, "   Superset of %d\n", numSets-1-k);
	    }
	    /* otherwise, row unique so far so look at next pair */
	    else {
	      k++;
	      stack_ptr = stack_ptr->next;
	    } 
	  } /* end while loop */

	  /* if no match was found, push set on stack and */
	  /* increment set counter, else do nothing */
	  if (!match) {
	    push_stack(sets[col], *inequal);
	    numSets++;
	  }
	  /* clear current set  */
	  ClearBitVector(inequal);

	}  /* end if checking for unequal row values */
      } /* end nested for loop stepping through rows */
    }

    /* done with this variable, so print out reduced variable dependency */
    /* sets. Do NOT pop stack so as to preserve structure for later */
    if (debug == TRUE) {
      fprintf(stderr, "Reduced sets for variable %d:\n", col);
      print_stack(sets[col]);
    }

    /* if more than one dependency set fnd, send them to branch and bound */
    /* and put these back in sets[][] array.  Must use a temp. array */
    /* to hold these values until Branch and Bound complete on the */
    /* old sets array */
    if (numSets > 1) {
      fprintf(stderr, "Variable %d: sorting sets", col);
      sort_stack(sets[col]);
      if (debug == TRUE) print_stack(sets[col]);
      fprintf(stderr, "  Using branch and bound to reduce sets:   ");
      temp_solns = BranchBound (sets[col], num_vars);
      kill_stack(sets[col]);   /* added 6/6/99 */
      sets[col] = temp_solns; 
      fprintf(stderr, "%d minimal set(s) found\n", stack_depth(sets[col]) );
      if (debug == TRUE) {
	fprintf(stderr, "Minimum input variables found:\n");
	print_stack(sets[col]);
	fprintf(stderr, "\n");
      }
    }
    else if (numSets == 1) 
      fprintf(stderr, "Variable %d: 1 set found\n\n", col);
    else {      /* numSets = 0, push zeros on stack as solution*/
      push_stack(sets[col], *inequal);
      fprintf(stderr, "Variable %d: no dependencies found  \n\n", col);
    }
  } /* end master loop stepping through columns (vars) */

  /* free memory */
  KillBitVector(inequal);
  KillBitVector(temp);
  KillBitVector(prev);
  free(inequal);
  free(temp);
  free(prev);

  return(sets);

} /* end function */

/*****************************************************************/

int main(int argc,char *argv[]) {

  int num_vars, num_conds;          /* number of variables + conditions */
  pertMatrixType pert;              /* input perturbation matrix */
  int **dag;                        /* directed acyclic graph */
  int **closure;                    /* transitive closure of dag */
  stack **sets;          /* array of stack pointers, one for each var. */
  FILE *fp1;
  int i, j;
  char *infile, *outfile, infofile[32];

  /* init variables and start code timer */
  fprintf(stderr, "\nDevelopment version 1.1; %s\n", __DATE__);
  debug = FALSE;
  all_outputs = FALSE;
  rand_outputs = FALSE;
  infile = NULL;
  outfile = NULL;
  limit_num = rand_num = 0;
  start_time();    /* start timer */

  /* parse command line, check to see if debug flag  */
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i], "-d") == 0) debug = TRUE ;
    else if (strcmp(argv[i], "-a") == 0) all_outputs = TRUE ;
    else if ( strcmp(argv[i], "-l") == 0 && i+1 < argc ) {
      limit = TRUE;
      limit_num = atoi(argv[++i]);
      fprintf(stderr, "Limiting to %d solutions\n", limit_num);
    }
    else if ( strcmp(argv[i], "-r") == 0 && i+1 < argc ) {
      rand_outputs = TRUE;
      rand_num = atoi(argv[++i]);
      fprintf(stderr, "Generating %d random solutions\n", rand_num);
    }
    else if (infile == NULL) infile = argv[i];
    else if (outfile == NULL) outfile = argv[i];
    else die (1) ;
  }
  if (infile == NULL || outfile == NULL) die (1); 
  sprintf(infofile, "%s.info", outfile);

  /* read input perturbation array */
  fprintf(stderr, "Reading input perturbation matrix\n");
  fp1 = fopen(infile, "r");
  if (fp1 == NULL) die (2);
  pert = ReadPerturbs(fp1, &num_conds, &num_vars);
  fclose(fp1);
  if (debug == TRUE) printPert(pert, num_conds, num_vars);
  fprintf(stderr, "%d conditions, %d nodes\n", num_conds, num_vars);

  /* construct a DAG incidence matrix for the data */
  fprintf(stderr, "Calculating directed acyclic graph (DAG)\n");
  dag = Perturb2DAG(pert, num_conds, num_vars);
  if (debug == TRUE) printMatrix(dag, num_vars, num_vars);

  /* find the transitive closure of the DAG */
  fprintf(stderr, "Calculating transitive closure of DAG\n");
  closure = FindClosure(dag, num_vars);
  if (debug == TRUE) printMatrix(closure, num_vars, num_vars);

  /* find the minimal set of inputs for each variable */
  sets = MinInputs(pert, closure, num_conds, num_vars);

  /* find specific functions for each variable */
  /* option -r overrides option -a */
  fprintf(stderr, "Calculating functions of minimal inputs\n");
  if (rand_outputs == TRUE) 
    MinFnRand(sets, pert, num_conds, num_vars, outfile);
  else if (all_outputs == TRUE) 
    MinFnAll(sets, pert, num_conds, num_vars, outfile); 
  else MinFn(sets, pert, num_conds, num_vars, outfile); 

  /* print run statistics */
  fprintf(stderr, "Writing run statistics to %s\n", infofile);
  SaveStats(infofile, sets, num_vars);

  /* free memory */
  for (i=0; i<num_vars; i++) {
    free(dag[i]);
    free(closure[i]);
    kill_stack(sets[i]);
  }
  free(dag);
  free(closure);
  free(sets);
  freePert(pert, num_conds);
}






