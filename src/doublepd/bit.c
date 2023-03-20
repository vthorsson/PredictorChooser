#include "bit.h"

const int NoBitsUInt = sizeof(unsigned int) * 8;

/* The purpose of this module is to use a bit vector to mimic a "normal"  */
/* integer array with entries being only 0 or 1.                          */

/* Use an unsigned integer as the basic unit of the bit vector            */
/* A bit vector may consist of more than one integers                     */
/* Assume the rightmost bit is bit 0                                      */
/* The leftmost bit is at position (sizeof(unsigned int)*8-1)             */
/* The first basic unit (unsigned int) making up the bit vector is        */ 
/*     indexed at 0                                                       */

/* allocate memory and return the bit vector with "no_slots" for 0/1 entries*/
/* and also initialize the allocated bit vector to 0                        */
/* allocate memory for the bit vector, but not the struct newV              */
/* returns TRUE if allocated                                                */
/* return FALSE if out of memory                                            */

BOOL NewBitVector (BitVectorType *newV, int no_slots) {
  int NoUnitsReq, i;

  /* to compute the number of basic units required for the bit vector */
  NoUnitsReq = (int) ceil ( (double)no_slots / (double) NoBitsUInt);
  
  newV->vector = (unsigned int *) malloc (sizeof(unsigned int) * NoUnitsReq);
  if (newV->vector == NULL) {
    return FALSE;
  } else {
    for (i=0; i< NoUnitsReq; i++)
      newV->vector[i] = 0;
    newV->NoBits = no_slots;
    newV->NoUnits = NoUnitsReq;
    return TRUE;
  } /*if*/
} /*NewBitVector*/


/* BitVectorIns: inserts a 1-bit into the given bit vector representing */
/* returns TRUE if the bit is inserted                                  */
/* returns FALSE if the bit is already there, ie, not inserted          */
/* parameters : bv -- the bit vector                                    */
/*              pos -- position to be inserted                          */
BOOL BitVectorIns (BitVectorType *bv, int pos )  {
  unsigned int temp;          /* temporary bit vector */
  int BitVectorNo;
  int PosNo;

  /* find the integer that we have to insert to */
  if (pos < 0) return FALSE;
  PosNo = pos % NoBitsUInt;
  BitVectorNo = pos / NoBitsUInt;
  if (BitVectorNo >= bv->NoUnits) {
    return FALSE;
  } /*if*/

  /* make a copy of the original unsigned int */
  temp = bv->vector[BitVectorNo];

  /* shift the bit vector until the bit to be inserted is rightmost */
  bv->vector[BitVectorNo] = bv->vector[BitVectorNo] >>  PosNo;

  /* check whether the bit is already on */
  if ( (bv->vector[BitVectorNo] & 1) == 1 ) {     /* bit already on */
    bv->vector[BitVectorNo] = temp;
    return FALSE;
  } /*if */

  /* insert the 1 bit */
  bv->vector[BitVectorNo] += 1;
  /* shift the bit vector back to its original relative position */
  bv->vector[BitVectorNo] = bv->vector[BitVectorNo] << PosNo ;
  /* shift to get the all the bits except the first pos rightmost bit */
  /* to be zero's                                                     */
  if ( PosNo > 0 ) {
    temp = temp << (NoBitsUInt - PosNo );
    temp = temp >> (NoBitsUInt - PosNo);
  } else {
    temp = 0;
  } /*if*/
  /* combine the two bit vectors */
  bv->vector[BitVectorNo] = bv->vector[BitVectorNo] + temp;

  return TRUE;  
} /* BitVectorIns */

/* BitVectorDel : deletes a 1-bit at posiiton pos                        */
/* returns TRUE if the bit is deleted                                    */
/* returns FALSE if the bit does not exist, ie, not deleted              */
/* parameters : bv -- the bit vector                                     */
/*              pos -- position to be deleted                            */
BOOL BitVectorDel (BitVectorType *bv, int pos )  {
  unsigned int temp;          /* temporary bit vector */
  int BitVectorNo;
  int PosNo;
  
  /* find the integer (basic unit ) that we have to delete*/
  if (pos < 0) return FALSE;
  PosNo = pos % NoBitsUInt;
  BitVectorNo = pos / NoBitsUInt;
  if (BitVectorNo >= bv->NoUnits) {
    return FALSE;
  } /*if*/

  /* make a copy of the original unsigned int */
  temp = bv->vector[BitVectorNo];
  
  /* shift the bit vector until the bit to be deleted is rightmost */
  bv->vector[BitVectorNo] = bv->vector[BitVectorNo] >>  PosNo;

  /* check whether the bit is already off */
  if ( (bv->vector[BitVectorNo] & 1) == 0 ) {    /* bit already off */
    bv->vector[BitVectorNo] = temp;
    return FALSE;
  } /*if */

  /* delete the 1 bit */
  bv->vector[BitVectorNo] -= 1;
  /* shift the bit vector back to its original relative position */
  bv->vector[BitVectorNo] = bv->vector[BitVectorNo] << PosNo ;
  /* shift to get the all the bits except the first pos rightmost bit */
  /* to be zero's                                                     */
  if ( PosNo > 0 ) {
    temp = temp << (NoBitsUInt - PosNo );
    temp = temp >> (NoBitsUInt - PosNo);
  } else {
    temp = 0;
  } /*if*/
  /* combine the two bit vectors */
  bv->vector[BitVectorNo] = bv->vector[BitVectorNo] + temp;

  return TRUE;  
} /* BitVectorDel */

/* BitVectorToggle : toggles a 1-bit at position pos                     */
/* returns TRUE if the bit is toggled                                    */
/* returns FALSE if the bit does not exist ( not toggled )               */
/* parameters : bv -- the bit vector                                     */
/*              pos -- position to be toggled                            */
BOOL BitVectorToggle (BitVectorType *bv, int pos )  {
  int BitState ; 
  int BitVectorNo;
  int PosNo;
  
  /* find the integer (basic unit ) that we have to toggle*/
  if (pos < 0) return FALSE;
  PosNo = pos % NoBitsUInt;
  BitVectorNo = pos / NoBitsUInt;
  if (BitVectorNo >= bv->NoUnits) {
    return FALSE;
  }/*if*/

  /* Determine if bit to be toggled is 0 or 1 */
  BitState = GetBitValue ( *bv, pos ) ; 

  /* toggle bit */
  if ( BitState == 0 ){
    BitVectorIns( bv, pos ) ;
  } else {
    BitVectorDel( bv, pos ) ;
  }

  return TRUE ; 

}/* BitVectorToggle */

/* returns the value of the given bit vector at position pos           */
/* return -1 if the pos is invalid                                     */
int GetBitValue (BitVectorType bv, int pos) {
  int BitVectorNo;
  int PosNo;
  int retval;
  unsigned int temp;
  
  /* find the integer (basic unit) that we have to find the value of */
  PosNo = pos % NoBitsUInt;
  BitVectorNo = pos / NoBitsUInt;
  if (BitVectorNo >= bv.NoUnits) {
    return -1;
  } /*if*/

  /* shift the bit vector until the bit is rightmost */
  temp = bv.vector[BitVectorNo] >>  PosNo;
  
  retval = (int) (temp & 1);
  return retval;
} /*GetBitValue*/


/* take the binary complement of the given bit vector       */
void ComplementBitVector (BitVectorType from_bv, BitVectorType *to_bv) {
  int i;

  for (i=0; i < from_bv.NoUnits; i++) {
    to_bv->vector[i] = ~(from_bv.vector[i]);
  } /*for*/
} /*ComplementBitVector*/


/* returns the number of one bit in a bit vector               */
int CountOneBitVector (BitVectorType bv) {
  int i, j, count = 0, remain;
  unsigned int temp;

  for (i=0; i < bv.NoUnits; i++) {
    temp = bv.vector[i];
    remain = bv.NoBits - i * NoBitsUInt;
    for (j=0; j <NoBitsUInt && j < remain; j++) {
      if ((temp & 1) == 1) {
	/* a 1-bit!! */
	count += 1;
      } /*if*/
      temp = temp >> 1;
    } /*for j*/
  } /*for i*/
  return count;
} /*CountOneBitVector*/

/* computes the bitwise AND of two bit vectors and returns to a third */
/* returns FALSE if all vectors are not of same length */
BOOL BitVectorAND (BitVectorType bv1, BitVectorType bv2, 
		   BitVectorType *bvOut)  {

  int i;

  if (bvOut->NoBits != bv1.NoBits || bvOut->NoBits != bv2.NoBits) 
    return FALSE;
  
  for (i=0; i < bvOut->NoUnits; i++)
    bvOut->vector[i] = bv1.vector[i] & bv2.vector[i];

  return TRUE;

}

/* computes the bitwise OR of two bit vectors and returns to a third */
/* returns FALSE if all vectors are not of same length */
BOOL BitVectorOR (BitVectorType bv1, BitVectorType bv2, 
		   BitVectorType *bvOut)  {

  int i;

  if (bvOut->NoBits != bv1.NoBits || bvOut->NoBits != bv2.NoBits) 
    return FALSE;
  
  for (i=0; i < bvOut->NoUnits; i++)
    bvOut->vector[i] = bv1.vector[i] | bv2.vector[i];

  return TRUE;

}


/* returns true if both bit vectors are equal, false otherwise */
BOOL BitVectorEQ (BitVectorType bv1, BitVectorType bv2) {
  int i;

  if (bv1.NoBits != bv2.NoBits)
    return FALSE;
  
  for (i=0; i < bv1.NoUnits; i++) {
    if (bv1.vector[i] != bv2.vector[i]) 
      return FALSE;
  }
  
  return TRUE;

}

/* clear bit vector */
void ClearBitVector (BitVectorType *bv) {
  int i;
  
  for (i=0; i< bv->NoUnits; i++)
      bv->vector[i] = 0;

} /*ClearBitVector*/

/* force bit vector to ones */
void OnesBitVector (BitVectorType *bv) {
  int i;
  
  for (i=0; i< bv->NoUnits; i++)
      bv->vector[i] = ~0;

} /* OnesBitVector*/


/* prints the bit vector */
void PrintBitVector (BitVectorType bv) {
  int i, x;

  for (i=0; i < bv.NoBits; i++) {
    x = GetBitValue (bv, i);
    if (i % 10 == 0)
      printf (" ");
    printf ("%d",x);
  }
}

/* prints the bit vector to a file */
void FilePrintBitVector (BitVectorType bv, FILE *fp) {
  int i, x;

  for (i=0; i < bv.NoBits; i++) {
    x = GetBitValue (bv, i);
    if (i % 10 == 0)
      fprintf (fp, " ");
    fprintf (fp, "%d",x);
  }
}

/* free up the memory taken by a bit vector                       */
void KillBitVector (BitVectorType *bv) {
  free (bv->vector);
  bv->vector = NULL;
  /*  free (bv); */
} /*KillBitVector*/

/* copies one vector to the other */
BOOL CopyBitVector (BitVectorType bv1, BitVectorType *bv2) {
  int i;

  if (bv2->NoBits != bv1.NoBits)
    return FALSE;

  for (i=0; i<bv1.NoUnits; i++) 
    bv2->vector[i] = bv1.vector[i];

  return TRUE;

}

/**************************************************************/
/* Inserts array of integers into BitVectorType               */
/*            : int_array array of 1-bits as integers         */
/*            : int_array[0] is the lowest value 1-bit        */


void IntArray2BitVector ( BitVectorType *bv, int *int_array ){

  int i ; 
  
  ClearBitVector( bv ) ; 

  for ( i=0 ; i<(bv->NoBits) ; i++ ){
    if ( int_array[i] == 1 ) BitVectorIns( bv, i) ; 
  }

}

/*******************************************************************/
/*  char2int_array - Converts character array of bits to
 *                   integer array of bits
 *         
 *                   b is the integer string returned and 
 *                   n is the number of bits
 *
 *  The lowest index ( leftmost ) character becomes the highest
 *  index ( n-1 ) in the integer array                             */

void char2int_array ( char *bit_string, int *b,  int n ){
  
  int i;
  double r;
  char a ; 

  for ( i=0 ; i< n ; i++ ){
    a = bit_string[ n-i-1 ] ;
    if ( a == '*' ) {
      /* if *, randomly assign 0 or 1 to a */
      r = (double) rand() / (double) RAND_MAX;
      if (r < 0.5) a = '0';
      else a = '1';
    }
    /*  on the basis that it does not affect deletion matrix */
    b[i] = a - '0';   /* convert from char to int */
    /* b[i] = atoi( &a ); */
  }
}

/******************************************************************/
/* Vesteinn Thorsson                          May 7, 1998
 *    
 *     link_bits.C - returns integer representation of binary array 
 *
 *                     input variables: 
 *                     b_dim - dimension of binary array 
 *                     *b    - pointer to binary array 
 *
 */
 
int link_bits ( int b_dim , int *b  ){ 

  int i, out; 
  
  out = b[ b_dim-1 ] ; 
  for ( i=0 ; i<b_dim-1; i++ ){ 
    out  <<=  1 ;   /*  shift bits to the left */
    out += b[ b_dim-2-i] ;   /* add rightmost bit */ 
  }
  return out ; 

}

/*****************************************************************/
