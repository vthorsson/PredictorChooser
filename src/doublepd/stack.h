#define __USE_FIXED_PROTOTYPES__
#ifndef __STACK_H__
#define __STACK_H__

/* stack implementation */
/* updated for doublepd by Trey Ideker 5/27/99 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "bit.h"

/* this stack contains both a data entry and the frequency */
/* or count of the data entry */

typedef struct stack_entry {
  struct stack_entry *next;
  BitVectorType data;
  int count;
} stack_entry;

typedef struct stack {
  stack_entry *top;
} stack;

stack *new_stack(void);

void kill_stack(stack *aStack);

BOOL push_stack(stack *aStack, BitVectorType bv);

BOOL pop_stack(stack *aStack, BitVectorType *bv, int *count);

BitVectorType top_of_stack(stack *aStack);

BOOL empty_stack (stack *aStack);

void clear_stack (stack *aStack);

void change_entry (stack_entry *entry, BitVectorType data);

BOOL already_in_stack (stack *aStack, BitVectorType bv);

void print_stack (stack *aStack);

int stack_depth (stack *aStack);

void sort_stack (stack *aStack);

void swap(stack_entry **, stack_entry **);


#endif
