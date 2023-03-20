/* Branch and Bound functions */
/* Trey Ideker 1/14/99 */

#include "BnB.h"

/*****************************************************************/
/* inputs a stack of inequalities and the number of variables,   */
/* uses a branch and bound algorithm to compute solution which   */
/* satisfies inequalities with minimum number of variables.      */
/* returns a stack of all solutions having a minimum # of vars.  */
/*                                                               */
/* modified 6/9/99 to limit search depth                         */
/*****************************************************************/

stack *BranchBound (stack *inequalSet, int num_vars) {
  
  BOOL retval;
  int min_cost;
  stack *result;
  tree_node *root;
  BitVectorType curr_soln;
  
  /* init. root tree */
  root = malloc(sizeof(tree_node));
  if (root == NULL) die(0);
  root->data = -1;
  root->LeftChild = NULL;
  root->RightSib = NULL;

  /* init stack and solution vector */
  result = new_stack();
  if (NewBitVector(&curr_soln, num_vars) == FALSE) die(0);
  min_cost = num_vars;
  
  /* recursively build and traverse tree */
  build_tree (root, inequalSet, inequalSet->top, result, 
	      &curr_soln, &min_cost);

  /* free memory */
  if (debug == TRUE) 
    fprintf(stderr, "Recursion finished: freeing tree memory...\n");
  KillBitVector(&curr_soln);
  free(root);

  return result;
}

/***************************************************************/

void build_tree (tree_node *curr_tree, stack *inequalSet, 
		 stack_entry *inequalSet_ptr, stack *result, 
		 BitVectorType *curr_soln, int *min_cost_ptr)   {
  
  int i, num_vars, curr_cost, min_cost;
  BOOL retval, firstVar;
  BitVectorType next;
  tree_node *tree_ptr;

  /* initialize some variables for this recursion only */
  /* assume number of bits in curr_soln is the number of vars */
  num_vars = curr_soln->NoBits;
  if (NewBitVector(&next, num_vars) == FALSE) die(0);
  curr_cost = CountOneBitVector(*curr_soln);
  
  if (debug == TRUE) {
    fprintf(stderr, "\nPerforming recursion...\n");
    fprintf(stderr, "  Curr_cost = %d,", curr_cost);
    fprintf(stderr, "  Min_cost = %d,", *min_cost_ptr);
    fprintf(stderr, "  Curr_soln =");
    PrintBitVector(*curr_soln);
    fprintf(stderr, "\n");
  }

  /* check 3 conditions for state of tree */
  if (satisfyTest(inequalSet, *curr_soln) == TRUE) {
    /* all inequalities satisfied: found a solution */
    /* if current cost is less than overall minimum, clear stack */
    /* and redefine min. cost */
    if (debug == TRUE) fprintf(stderr, "All inequalities satisfied\n");
    if (curr_cost < *min_cost_ptr) {
      clear_stack(result);
      *min_cost_ptr = curr_cost;
    }
    /* if the new_data for the current path is not on the  */
    /* result stack, push the new_data onto the result stack     */
    if (already_in_stack (result, *curr_soln) == FALSE) {
      push_stack (result, *curr_soln); 
    }

  } else if ( empty_stack(result) == FALSE && curr_cost >= *min_cost_ptr) {
    /* current solution is more costly than others found */
    /* abort the current path */
    if (debug == TRUE) fprintf(stderr, "Aborting path\n");
    
  } else {
    /* current cost <= min. cost found, but not all inequalities satisfied */
    /* get the next inequality to be satisfied... */
    CopyBitVector(inequalSet_ptr->data, &next);
    if (debug == TRUE) {
      fprintf(stderr, "Adding next inequality to tree: ");
      PrintBitVector(next);
      fprintf(stderr, "\n");
    }
    /* and add the variables in inequality "next" to the tree. */
    tree_ptr = curr_tree;
    firstVar = TRUE;
    for (i=0; i<num_vars; i++) {
      if (GetBitValue(next, i) == 1) {
	if (firstVar == TRUE) {
	  /* first variable added goes to left child */
	  tree_ptr->LeftChild = malloc(sizeof(tree_node));
	  if (tree_ptr->LeftChild == NULL) die(0);
	  tree_ptr = tree_ptr->LeftChild;
	  /* insure elements of new structure are initialized */
	  tree_ptr->data = -1;
	  tree_ptr->LeftChild = NULL;
	  tree_ptr->RightSib = NULL;
	  firstVar = FALSE;
	} else {
	  /* subsequent variables go to right sib */
	  tree_ptr->RightSib = malloc(sizeof(tree_node));
	  if (tree_ptr->RightSib == NULL) die(0);
	  tree_ptr = tree_ptr->RightSib;
	  /* insure elements of new structure are initialized */
	  tree_ptr->data = -1;
	  tree_ptr->LeftChild = NULL;
	  tree_ptr->RightSib = NULL;
	}
	/* if not part of current solution, add var to tree */
	if (GetBitValue(*curr_soln, i) == 0) tree_ptr->data = i;
      }
    } /* end for (i=0; i<num_vars... */
    /* add current var. to solution, and continue traversing the tree */
    /* with the leftmost child of the current node. If var. not */
    /* part of solution, increment current cost */
    retval = BitVectorIns(curr_soln, curr_tree->LeftChild->data);
    if (retval == TRUE) ++curr_cost;
    build_tree (curr_tree->LeftChild, inequalSet, inequalSet_ptr->next, 
		result, curr_soln, min_cost_ptr);
  } /*if*/

  /* remove most recent variable from solution and decrement cost*/
  retval = BitVectorDel(curr_soln, curr_tree->data);
  if (retval == TRUE) --curr_cost;

  /* recurse on the next sibling if it exists */
  if (curr_tree != NULL && curr_tree->RightSib != NULL) {
    /* add current variable to solution and continue with right */
    /* sibling.  If not already part of solution, increase cost */
    /* do NOT increase set pointer */
    retval = BitVectorIns(curr_soln, curr_tree->RightSib->data);
    if (retval == TRUE) ++curr_cost;
    build_tree (curr_tree->RightSib, inequalSet, inequalSet_ptr, 
		result, curr_soln, min_cost_ptr);
  } /*if*/  

/* clean up local variables and tree */
  KillBitVector(&next);
  free(curr_tree->RightSib);
  free(curr_tree->LeftChild);

} /* function */

/*****************************************************************/
/* code to see if the variables represented in bit vector data */
/* satisfy the set of inequalities */

BOOL satisfyTest (stack *inequalSet, BitVectorType data) {
  int i, ones, num_vars;
  BitVectorType *inequal, *temp;
  stack_entry *stack_ptr;
  BOOL retval;

  /* assume sets and data are same length bit vector */
  /* extract num_vars from data bit vector */
  num_vars = data.NoBits;

  /* initialize variables */
  inequal = malloc(sizeof(BitVectorType));
  temp = malloc(sizeof(BitVectorType));
  if (inequal == NULL || temp == NULL) die(0);
  if (NewBitVector(inequal, num_vars) == FALSE) die(0);
  if (NewBitVector (temp, num_vars) == FALSE) die(0);

  /* initialize pointers and ones counter */
  stack_ptr = inequalSet->top;
  ones = 1;

  /* core while loop to iterate through stack */
  while (stack_ptr != NULL && ones != 0) {
    CopyBitVector(stack_ptr->data, inequal);
    BitVectorAND(*inequal, data, temp);
    ones = CountOneBitVector(*temp);
    stack_ptr = stack_ptr->next;
  }

  /* free memory before exiting */
  KillBitVector(inequal);
  KillBitVector(temp);
  free(inequal);
  free(temp);

  /* test if AND was successful with each inequality */
  if (ones == 0) return FALSE;
  else return TRUE;
  
}

/*****************************************************************/
/* recursively free memory occupied by tree */

void KillTree (tree_node *aTree) {
  
  if (aTree->LeftChild != NULL) {
    KillTree(aTree->LeftChild);
    free(aTree->LeftChild);
  }
  if (aTree->RightSib != NULL) {
    KillTree(aTree->RightSib);
    free(aTree->RightSib);
  }
}
