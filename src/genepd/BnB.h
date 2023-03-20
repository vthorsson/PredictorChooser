#ifndef __BnB_h__ 
#define __BnB_h__

#include "stack.h"
#include "bit.h"
#include "util.h"
#include "main.h"

/* structures */
typedef struct tree_node {
  int data;
  struct tree_node *LeftChild;
  struct tree_node *RightSib;
} tree_node;

/* fn definitions */
stack *BranchBound (stack *inequalSet, int num_vars);
void build_tree (tree_node *curr_tree, stack *inequalSet, 
		 stack_entry *inequalSet_ptr, stack *result,
		 BitVectorType *curr_soln, int *min_cost_ptr);
BOOL satisfyTest (stack *, BitVectorType);
void KillTree (tree_node *aTree);


#endif 
