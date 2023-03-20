#include "bit.h"
#include "stack.h"
#include "util.h"
#include "BnB.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

main () {
  BitVectorType v1;
  BitVectorType v2;
  BitVectorType v3;
  BitVectorType v4;
  BitVectorType v5;
  BOOL retval;
  int i, x;
  int *m, *n;
  stack *test;
  BOOL test2;

  retval = NewBitVector (&v1, 10);
  retval = NewBitVector (&v2, 10);
  retval = NewBitVector (&v3, 10);
  retval = NewBitVector (&v4, 10);
  retval = NewBitVector (&v4, 10);
  BitVectorIns(&v1, 0);
  BitVectorIns(&v2, 1);
  BitVectorIns(&v3, 2);
  BitVectorIns(&v4, 3);
  fprintf(stderr, "v1 = ");
  PrintBitVector(v1);
  fprintf(stderr, "\nv2 = ");
  PrintBitVector(v2);
  fprintf(stderr, "\nv3 = ");
  PrintBitVector(v3);
  fprintf(stderr, "\nv4 = ");
  PrintBitVector(v4);
  fprintf(stderr, "\n");

  test = new_stack();
  push_stack(test, v1);
  push_stack(test, v2);
  push_stack(test, v3);
  push_stack(test, v4);
  fprintf(stderr, "Stack = \n");
  print_stack(test);
  fprintf(stderr, "\n");
  pop_stack(test, &v3);
  pop_stack(test, &v3);
  fprintf(stderr, "v3 = ");
  PrintBitVector(v3);
  push_stack(test, v3);
  fprintf(stderr, "\nStack = \n");
  print_stack(test);
  fprintf(stderr, "\n");
  BitVectorIns(&v3, 1);
  fprintf(stderr, "v3 = ");
  PrintBitVector(v3);
  fprintf(stderr, "\nStack = \n");
  print_stack(test);

  kill_stack (test);

} /*main*/





