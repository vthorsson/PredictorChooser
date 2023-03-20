#ifndef __BIT_H__
#define __BIT_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "util.h"

/* The purpose of this module is to use a bit vector to mimic a "normal"  */
/* integer array with entries being only 0 or 1.                          */

/* Use an unsigned integer as the basic unit of the bit vector            */
/* A bit vector may consist of more than one integers                     */
/* Assume the rightmost bit is bit 0                                      */
/* The leftmost bit is at position (sizeof(unsigned int)*8-1)             */
 
/* size of a basic unit of the bit vector                                 */
/* const int NoBitsUInt = sizeof(unsigned int)*8; */

typedef struct BitVectorType {
  unsigned int *vector;
  int NoBits;
  int NoUnits;
} BitVectorType;

BOOL NewBitVector (BitVectorType *newV, int no_slots);

BOOL BitVectorIns (BitVectorType *bv, int pos );

BOOL BitVectorDel (BitVectorType *bv, int pos );

int GetBitValue (BitVectorType bv, int pos);

void ComplementBitVector (BitVectorType from_bv, BitVectorType *to_bv);

int CountOneBitVector (BitVectorType bv);

BOOL BitVectorAND (BitVectorType bv1, BitVectorType bv2, BitVectorType *bvOut);

BOOL BitVectorOR (BitVectorType bv1, BitVectorType bv2, BitVectorType *bvOut);

BOOL BitVectorEQ (BitVectorType bv1, BitVectorType bv2) ;

void ClearBitVector (BitVectorType *bv);

void PrintBitVector (BitVectorType bv);

void KillBitVector (BitVectorType *bv);

BOOL CopyBitVector (BitVectorType bv1, BitVectorType *bv2);

#endif
