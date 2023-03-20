stack ** MinInputs (int **pert, int **closure, int num_vars) {

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
    for (row = 0; row < num_vars+1; row++) {
      for (row2 = row+1; row2 < num_vars+1; row2++) {
	/* search for rows which differ in logic value, but
	   skip row if it is the perturbation of the var. in question */
	if ( (col != row-1) &&
	     (col != row2-1) &&
	     (pert[row][col] != pert[row2][col]) ) {
	  /* when found, search all columns in that row for a 
	     corresponding change in logic value */
	  for (col2 = 0; col2 < num_vars; col2++) {
	    /* consider a column (variable) only if it is not reachable
	       in the transitive closure matrix */
	    if ( (pert[row][col2] != pert[row2][col2]) &&
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

/*************************************************************/



/*************************************************************/

void build_net(stack **sets, int **pert, int **pred, 
	       char **fn, int var, int num_vars, char *outfile_base, 
	       int *outfile_num) {

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
    addSoln(*inputs, pert, pred, fn, var, num_vars);
    build_net(sets, pert, pred, fn, var+1, num_vars, 
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
      build_net(sets, pert, pred, fn, var, num_vars, 
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

void addSoln (BitVectorType inputs, int **pert, int **pred, char **fn, 
	      int var, int num_vars) {
  
  int i, k, size, row2, col2;
  int col = var;     /* variable num is simply column in pred matrix */
  unsigned int index;

  /* set prediction matrix values */
  for (i=0; i<num_vars; i++) {
    if (GetBitValue(inputs,i) == 1) {
      if (debug == TRUE) fprintf(stderr, "[%d] ", i);
      pred[i][col] = 1;
    }
    else pred[i][col] = 0;
  }    
  /* set function string to starting state of all dont cares (*).
     size of function is 2 ^ number of sets, which is given by i.  
     Mark the end of the function list by inserting a bar (|) */
  size = pow(2, CountOneBitVector(inputs) );
  if (fn[col] != NULL) free(fn[col]);
  fn[col] = calloc(size+1, sizeof(char));
  if (fn[col] == NULL) die(0);
  for (k=0; k < size; k++) fn[col][k] = '*';
  fn[col][k] = '|';
  
  /* now that we know indep. variables, determine function */
  for (row2=0; row2 < num_vars+1; row2++) {
    /* skip row if its the perturbation due to the dependent variable */
    if (row2 != col+1) {
      /* calculate index */
      k = 0;
      index = 0;
      for (col2=0; col2 < num_vars; col2++) {
	if ( GetBitValue(inputs, col2) == 1 ) 
	  index = index | (pert[row2][col2] << k++);
      } /* end col2 for loop */
      /* with index calculated, set function */
      if (pert[row2][col] == 1) fn[col][index] = '1';
      else fn[col][index] = '0';
    }
  } /* end row2 for loop */
  
  /* print out the final function */
  if (debug == TRUE) {
    fprintf(stderr, "\nFunction: ");
    for (k=size-1; k >= 0; k--) fprintf(stderr, "%c", fn[col][k]);
    fprintf(stderr, "\n\n");
  }
}
