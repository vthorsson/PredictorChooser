#include "bit.h"
#include "stack.h"
#include "BnB.h"
#include "util.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

main () {
  BitVectorType v1;
  BitVectorType v2;
  BitVectorType v3;
  BitVectorType v4;
  BitVectorType *v5;
  BitVectorType *v6;
  BOOL retval;
  int i, x;
  int *m, *n;
  stack *test;
  BOOL test2;

  retval = NewBitVector (&v1, 100);
  if (retval == FALSE)
    printf ("ERROR: new bit vector v1 false\n");
  printf ("\n v1 is now printed after new:\n");
  PrintBitVector(v1);
  printf("\n");

  retval = NewBitVector (&v2, 20);
  if (retval == FALSE)
    printf ("ERROR: new bit vector v2 false\n");
  printf ("\n v2 is now printed: after new\n");
  PrintBitVector(v2);
  printf("\n");

  /* test bit insert */
  retval = BitVectorIns (&v1, 0);
  if (retval == FALSE)
    printf ("ERROR: insert v1 false 0\n");
  printf ("\n v1 is now printed after in 0:\n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");
  
  retval = BitVectorIns (&v1, 1);
  if (retval == FALSE)
    printf ("ERROR: insert v1 false 1\n");
  printf ("\n v1 is now printed: after in 1\n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");
  
  retval = BitVectorIns (&v1, 31);
  if (retval == FALSE)
    printf ("ERROR: insert v1 false 31\n");
  printf ("after ins 31, bit 31 is %d\n", GetBitValue (v1, 31));
  printf ("\n v1 is now printed: after ins 31\n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");

  retval = BitVectorIns (&v1, 32);
  if (retval == FALSE)
    printf ("ERROR: insert v1 false 32\n");
  printf ("\n v1 is now printed:after ins 32\n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");
  
  retval = BitVectorIns (&v1, 50);
  if (retval == FALSE)
    printf ("ERROR: insert v1 false 50\n");
  printf ("\n v1 is now printed: after ins 50\n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");

  retval = BitVectorIns (&v1, 0);
  if (retval == FALSE)
    printf ("GOOD: insert v1 false 0 again\n");

  /* print out v1 */  
  printf ("\n v1 is now printed:\n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");

  x = CountOneBitVector (v1);
  printf ("\n# one bit in v1 is %d\n", x);

  /* test complement now */
  ComplementBitVector (v1, &v1);
  printf ("\n after first complement: \n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");  

  /* test bit del */
  retval = BitVectorDel (&v1, 0);
  if (retval == FALSE)
    printf ("ERROR: del v1 false 0\n");
  retval = BitVectorDel (&v1, 1);
  if (retval == FALSE)
    printf ("ERROR: del v1 false 1\n");
  retval = BitVectorDel (&v1, 31);
  if (retval == FALSE)
    printf ("ERROR: del v1 false 31\n");
  retval = BitVectorDel (&v1, 32);
  if (retval == FALSE)
    printf ("ERROR: del v1 false 32\n");
  retval = BitVectorDel (&v1, 50);
  if (retval == FALSE)
    printf ("ERROR: del v1 false 50\n"); 
  retval = BitVectorIns (&v1, 0); 
  if (retval == FALSE)
    printf ("ERROR: ins v1 false 0\n");

  /* print out v1 */  
  printf ("\n v1 is now printed:\n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");

  /* test bit insert */
  retval = BitVectorIns (&v2, 0);
  if (retval == FALSE)
    printf ("ERROR: insert v2 false 0\n");
  retval = BitVectorIns (&v2, 11);
  if (retval == FALSE)
    printf ("ERROR: insert v2 false 11\n");
  retval = BitVectorIns (&v2, 31);
  if (retval == FALSE)
    printf ("GOOD: del 31 from v2\n");

  printf ("\n v2 is now printed:\n");
  for (i=0; i < 20; i++) {
    x = GetBitValue (v2, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");

  x = CountOneBitVector (v2);
  printf ("\n# one bit in v2 is %d\n", x);

  /* test bit del */
  retval = BitVectorDel (&v2, 0);
  if (retval == FALSE)
    printf ("ERROR: del v2 false 0\n");
  retval = BitVectorDel (&v2, 11);
  if (retval == FALSE)
    printf ("ERROR: del v2 false 11\n");

  /* print out v2 */  
  printf ("\n v2 is now printed:\n");
  for (i=0; i < 20; i++) {
    x = GetBitValue (v2, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");

  /* test complement now */
  ComplementBitVector (v1, &v1);
  printf ("\n after first complement: \n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n");  

  ComplementBitVector (v1, &v1);
    printf ("\n after second complement: \n");
  for (i=0; i < 100; i++) {
    x = GetBitValue (v1, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
  printf ("\n"); 

  x = CountOneBitVector (v1);
  printf ("\n# one bit in v1 is %d\n", x);
  x = CountOneBitVector (v2);
  printf ("\n# one bit in v2 is %d\n", x);

  printf("\nTest AND operation\n v2: ");

  retval = NewBitVector (&v3, 20);
  retval = NewBitVector (&v4, 20);

  retval = BitVectorIns (&v3, 0);
  retval = BitVectorIns (&v3, 12);
  retval = BitVectorIns (&v3, 6);
  retval = BitVectorIns (&v3, 3);
  retval = BitVectorIns (&v2, 6);
  retval = BitVectorIns (&v2, 10);
  retval = BitVectorIns (&v2, 12);  

  PrintBitVector(v2);
  printf("\n v3: ");
  PrintBitVector(v3);
  printf("\n v4: ");
  PrintBitVector(v4);
  printf("\n v4: ");
  BitVectorAND(v2, v3, &v4);
  PrintBitVector(v4);
  printf(" (after AND)\n");

/* test stack usage */

  printf("\nTest Stack Usage\n");
  printf("Pushing v2 then v3, then popping twice.\n");
  test = new_stack();
  retval = push_stack(test, v2);
  retval = push_stack(test, v3);
  retval = pop_stack(test, &v4);
  PrintBitVector(v4);
  printf("\n");
  retval = pop_stack(test, &v4);
  PrintBitVector(v4);
  printf("\nNow Pushing again\n");
  retval = push_stack(test, v4);
  PrintBitVector(v4);
  printf("\n");
  kill_stack(test); 

/* test pointer to BitVector */

  printf("\nTest Pointer to BitVector \n");
  v5 = malloc(sizeof(BitVectorType));
  retval = NewBitVector (v5, 5);
  if (retval == FALSE)
    printf ("ERROR: new bit vector v5 false\n");
  printf ("\n v5 is now printed after new:\n");
  PrintBitVector(*v5);
  printf("\n");

/* test clearstack function */

  printf("\nTest ClearBitVector Function\n");
  retval = NewBitVector(v6, 5);
  if (retval == FALSE)
    printf ("ERROR: new bit vector v6 false\n");
  printf ("\n v6 is now printed after new and adding 1 bit:\n");
  BitVectorIns(v6, 3);
  PrintBitVector(*v6);
  printf("\n v6 is now printed after clearing v5!!!\n");
  ClearBitVector(v5);
  PrintBitVector(*v6);
  printf("\n");

/* test reassignment */
  printf("\nTest reassignment\n");
  printf("v2 and v3: \n");
  PrintBitVector(v2);
  printf("\n");
  PrintBitVector(v3);
  printf("\nCopy v2 to v3 and then alter v2\n");
  printf("v2 and v3: \n");
  CopyBitVector(v2, &v3);
  KillBitVector(&v2);
  retval = NewBitVector(&v2,20);
  PrintBitVector(v2);
  printf("\n");
  PrintBitVector(v3);
  printf("\n");

/* test Branch and Bound algorithms */
  printf("\nTest Branch and Bound algorithms\n");
  BitVectorIns(&v2, 6);
  BitVectorDel(&v4, 6);
  printf("v2, v3, and v4: \n");
  PrintBitVector(v2);
  printf("\n");
  PrintBitVector(v3);
  printf("\n");
  PrintBitVector(v4);
  printf("\nPush v2, v3 on stack, see if v2 satisfies\n"); 
  test = new_stack();
  retval = push_stack(test, v2);
  retval = push_stack(test, v3);
  retval = satisfyTest(test, v2);
  if (retval == FALSE) printf("returned false\n");
  else printf("returned true\n");
  printf("See if v4 satisfies\n");
  retval = satisfyTest(test, v4);
  if (retval == FALSE) printf("returned false\n");
  else printf("returned true\n");
  printf("Test to see if v2 already_in_stack\n");
  retval = already_in_stack(test, v2);
  if (retval == TRUE) printf("good: found in stack\n");
  printf("Test to see if v4 already_in_stack\n");
  retval = already_in_stack(test, v4);
  if (retval == FALSE) printf("good: not in stack\n");
  printf("Pop to assure integrity of stack\n");
  PrintBitVector(v4);
  printf("\n");
  retval = pop_stack(test, &v4);
  PrintBitVector(v4);
  printf("\n");
  retval = pop_stack(test, &v4);
  if (retval == FALSE) printf("good: Attempt to Pop again failed.\n");

} /*main*/





