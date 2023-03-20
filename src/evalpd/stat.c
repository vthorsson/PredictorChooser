#include "stat.h"


/************************************************************/

void initStat (statType *stat) {

  stat->numSolns = 0.0;
  stat->simEdges = 0.0;
  stat->predEdges = 0.0;
  stat->shared = 0.0;
  stat->percent = 0.0;
  stat->simFound = 0.0;
  stat->oneSolns = 0.0;
  stat->cycles = 0.0;
  stat->time = 0.0;

}

/************************************************************/
/* adds a new stat structure to an old one                  */

void accumStat (statType newStat, statType *oldStat) {

  oldStat->numSolns += newStat.numSolns;
  oldStat->simEdges += newStat.simEdges;
  oldStat->predEdges += newStat.predEdges;
  oldStat->shared += newStat.shared;
  oldStat->percent += newStat.percent;
  oldStat->simFound += newStat.simFound;
  oldStat->oneSolns += newStat.oneSolns;
  oldStat->cycles += newStat.cycles;
  oldStat->time += newStat.time;

}

/************************************************************/
/* subtracts a new stat structure from an old one           */

void subtractStat (statType *statOut, statType stat1, statType stat2) {

  statOut->numSolns = stat1.numSolns - stat2.numSolns;
  statOut->simEdges = stat1.simEdges - stat2.simEdges;
  statOut->predEdges = stat1.predEdges - stat2.predEdges;
  statOut->shared = stat1.shared - stat2.shared;
  statOut->percent = stat1.percent - stat2.percent;
  statOut->simFound = stat1.simFound - stat2.simFound;
  statOut->oneSolns = stat1.oneSolns - stat2.oneSolns;
  statOut->cycles = stat1.cycles - stat2.cycles;
  statOut->time = stat1.time - stat2.time;

}

/************************************************************/
/* copies one statType to another statType             */

void copyStat(statType stat1, statType *stat2) {

  stat2->numSolns = stat1.numSolns;
  stat2->simEdges = stat1.simEdges;
  stat2->predEdges = stat1.predEdges;
  stat2->shared = stat1.shared;
  stat2->percent = stat1.percent;
  stat2->simFound = stat1.simFound;
  stat2->oneSolns = stat1.oneSolns;
  stat2->cycles = stat1.cycles;
  stat2->time = stat1.time;

}

/*************************************************************/
/* divides each element in a stat structure by a number     */

void divStat (statType *stat, double number) {

  stat->numSolns /= number;
  stat->simEdges /= number;
  stat->predEdges /= number;
  stat->shared /= number;
  stat->percent /= number;
  stat->simFound /= number;
  stat->oneSolns /= number;
  stat->cycles /= number;
  stat->time /= number;

}

/************************************************************/
/* takes each element of the statType to power "number" */

void powStat (statType *statOut, statType statIn, double number) {

  statOut->numSolns = pow(statIn.numSolns, number);
  statOut->simEdges = pow(statIn.simEdges, number);
  statOut->predEdges = pow(statIn.predEdges, number);
  statOut->shared = pow(statIn.shared, number);
  statOut->percent = pow(statIn.percent, number);
  statOut->simFound = pow(statIn.simFound, number);
  statOut->oneSolns = pow(statIn.oneSolns, number);
  statOut->cycles = pow(statIn.cycles, number);
  statOut->time = pow(statIn.time, number);

}

/************************************************************/
/* prints a stat structure                                  */

void printStat (statType stat) {

  fprintf(stderr, "NumSolns=%5.2f, SimEdges=%5.2f, PredEdges=%5.2f, \n",
	  stat.numSolns, stat.simEdges, stat.predEdges);
  fprintf(stderr, "  Shared=%5.2f, Percent=%5.2f, SimFound=%5.2f,  \n",
	  stat.shared, stat.percent, stat.simFound);
  fprintf(stderr, "  OneSolns=%5.2f, Cycles=%5.2f, Time=%5.2f\n", 
	  stat.oneSolns, stat.cycles, stat.time);

}

/*************************************************************/
/* computes sample mean, sample stdev, and std err for stats */
/* N is the population size */

void computeStat (statType *statVector, int N, statType *mean,
		  statType *stdev, statType *standerr) {

  int i, usableN = N;
  statType temp, temp2;

  /* initialize variables */
  initStat(mean);
  initStat(stdev);
  initStat(standerr);

  /* compute the sample mean, but first check to see if any predicted */
  /* networks found.  If no found, then predEdges will equal 0 */
  for (i=0; i < N; i++) {
    if (statVector[i].predEdges > 0.0)
      accumStat(statVector[i], mean);
    else {
      fprintf(stderr, "Excluding # %d from statistics\n", i);
      usableN--;;
    }
  }
  divStat(mean, usableN );

  /* compute the sample stdev */
  for (i=0; i < N; i++) {
    if (statVector[i].predEdges > 0.0) {
      initStat(&temp);
      initStat(&temp2);
      subtractStat(&temp, statVector[i], *mean);
      powStat(&temp2, temp, 2.0);
      accumStat(temp2, stdev);
    }
  }
  divStat(stdev, usableN-1);
  copyStat(*stdev, &temp);
  powStat(stdev, temp, 0.5); 
  
  /* compute the std err */
  copyStat(*stdev, standerr);
  divStat (standerr, sqrt(usableN));

}


