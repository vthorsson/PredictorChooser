#ifndef __STAT_H__
#define __STAT_H__

#include <stdio.h>
#include <math.h>

/* structures */
typedef struct {
  double numSolns;
  double simEdges;
  double predEdges;
  double shared;
  double percent;
  double simFound;
  double oneSolns;
  double cycles;
  double time;
}
statType;

void initStat (statType *);
void accumStat (statType, statType *);
void subtractStat (statType *, statType, statType);
void powStat (statType *, statType, double);
void copyStat(statType, statType *);
void divStat (statType *, double);
void printStat (statType);
void computeStat (statType *, int, statType *, statType *, statType *);

#endif
