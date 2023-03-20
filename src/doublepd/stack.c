/* stack code by Kayee Yeung */
/* Modified by Trey Ideker 1/7/99 */

#include "stack.h"

/* to create a new stack                               */
/* returns NULL if out of memory                       */
stack *new_stack(void)
{
  stack *new_item = calloc(sizeof(stack),1);
  if(new_item == NULL)
    return NULL;
  return new_item;
} /*new_stack*/


/*
 *      Kill the stack without regard for elements.  They must be cleaned away
 *      before this is called or we leak memory.
 */
void kill_stack(stack *aStack)
{

  clear_stack(aStack);
  free(aStack->top);
  aStack->top = NULL;
  free(aStack);
  aStack = NULL;

} /*kill_stack*/

/* To push an item on the given stack                          */
/* returns FALSE if out of memory                              */
/* initializes count to 1                                      */
BOOL push_stack(stack *aStack, BitVectorType bv)
{
  stack_entry *prev_top = aStack->top;
  BOOL retval;

  if(!(aStack->top = calloc(sizeof(stack_entry),1))) {
    aStack->top = prev_top;
    return FALSE;
  } else {

    aStack->top->next = prev_top;
    retval = NewBitVector(&aStack->top->data, bv.NoBits);
    CopyBitVector(bv, &aStack->top->data);
    aStack->top->count = 1;
    return TRUE;
  } /*if*/
} /*push_stack*/

/* To pop an item off from the stack */
/* Modified 5/27/99 by Trey Ideker to also return count */
BOOL pop_stack(stack *aStack, BitVectorType *bv, int *count)
{
  stack_entry *orig_top = aStack->top;

  if(!orig_top) 
    return FALSE;

  aStack->top = orig_top->next;
  CopyBitVector(orig_top->data, bv);
  KillBitVector(&orig_top->data);
  *count = orig_top->count;
  free(orig_top);
  return TRUE;
} /*pop_stack*/


/* to return the top of the stack */
BitVectorType top_of_stack(stack *aStack)
{
  assert(aStack);
  assert(aStack->top);

  return aStack->top->data;
} /*top_of_stack*/


/* returns TRUE if aStack is empty       */
BOOL empty_stack (stack *aStack) {
  assert (aStack);

  if (aStack->top == NULL) 
    return (TRUE);
  else
    return (FALSE);
} /*empty_stack*/

/* clears all entries from a stack without killing it */
void clear_stack (stack *aStack) {
  
  stack_entry *orig_top;

  assert (aStack);
  while ( aStack->top != NULL ) {
    orig_top = aStack->top;
    KillBitVector(&orig_top->data);
    aStack->top = aStack->top->next;
    free(orig_top);
    orig_top = NULL;
  } /*while*/
  
}

/* changes a stack entry referenced by a stack entry pointer */
void change_entry (stack_entry *entry, BitVectorType bv) {

  CopyBitVector(bv, &entry->data);

}

/* test to see if at bit vector is already present in stack */
/* updated 5/27/99 by Trey Ideker to update count if found */

BOOL already_in_stack (stack *aStack, BitVectorType bv) {

  stack_entry *stack_ptr = aStack->top;
  BOOL present = FALSE;

  while (stack_ptr != NULL && present == FALSE) {
    present = BitVectorEQ(stack_ptr->data, bv);
    if (present == TRUE) stack_ptr->count++; 
    stack_ptr = stack_ptr->next;
  }

  return present;
}

/* prints a stack: the bitVector and the count */
/* modified 5/27/99 by Trey Ideker */

void print_stack (stack *aStack) {
  
  int i=0;
  stack_entry *stack_ptr;
  
  stack_ptr = aStack->top;
  while (stack_ptr != NULL) {
    printf("   [%d]: ",i);
    PrintBitVector(stack_ptr->data);
    printf(" Count = %d\n", stack_ptr->count);
    stack_ptr = stack_ptr->next;
    i++;
  }
}

/* calculates and returns the depth of a stack */
int stack_depth (stack *aStack) {
  
  int depth=0;
  stack_entry *stack_ptr;

  stack_ptr = aStack->top;
  while (stack_ptr != NULL) {
    stack_ptr = stack_ptr->next;
    depth++;
  }

  return depth;
}

/* sort a stack of BitVectors by the number of ones in each BitVector */
/* in ascending order */
void sort_stack (stack *aStack) {

  int i, j, depth = 0;
  int one, two;
  stack_entry *stack_ptr, **ptr_array;
  
  /* find depth of stack */
  depth = stack_depth(aStack);
  ptr_array = calloc (sizeof(stack_entry *), depth);

  /* extract address of each element */
  stack_ptr = aStack->top;
  fprintf(stderr, "\n");
  for (i=0; i<depth; i++) {
    ptr_array[i] = stack_ptr;
    stack_ptr = stack_ptr->next;
  }

  /* sort address array */
  for (i = 0; i < depth-1; i++) 
    for (j = depth-1; j > i; j--) {
      one = CountOneBitVector(ptr_array[j-1]->data);
      two = CountOneBitVector(ptr_array[j]->data);
      if (one > two) swap (&ptr_array[j-1], &ptr_array[j]);
    }

  /* rebuild stack in new order */
  aStack->top = ptr_array[0];
  stack_ptr = aStack->top;
  for (i=1; i<depth; i++) {
    stack_ptr->next = ptr_array[i];
    stack_ptr = stack_ptr->next;
  }
  stack_ptr->next = NULL;

  /* free memory */
  free(ptr_array);
}

void swap(stack_entry **sp1, stack_entry **sp2) {
  
  stack_entry *temp;

  temp = *sp1;
  *sp1 = *sp2;
  *sp2 = temp;
}
