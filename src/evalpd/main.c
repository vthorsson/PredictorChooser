/* Evaluation of gene prediction of a known, simulated network */
/* January 1999 */
/* Copyright (c) Trey Ideker and Vesteinn Thorsson */

/* Options: -d = debug mode        -h = suppress headers      */
/**************************************************************/

#include "main.h"

/**************************************************************/
/* nwRead returns adjacency matrix describing a network graph */
/* matrix is read from file and stored in 2D array matOut     */
/* also stores number of variables read in num_nodes          */

int ** nwRead (FILE *fp, int *vars)
{
  int i, j;
  int *temp, **matOut;
  char line[LINESIZE] = "init";
  
  /* look for num_nodes tag to get size of matrix */
  while ( strncmp( line, "num_nodes", 9) != 0 ) {
    fscanf(fp, "%s", line);
  }  
  fscanf(fp, "%d", vars);

  /* look for Graph tag to get matrix itself*/
  while (strcmp(line, "Graph") != 0 ) {
    fscanf(fp, "%s", line);
  }
  /* read in matrix values */
  matOut = calloc (*vars, sizeof(int *));
  if (matOut == NULL) die(0);
  for(i=0;i<*vars;i++) {
    matOut[i] = calloc (*vars, sizeof(int));
    if(matOut[i] == NULL) die(0);
    temp = matOut[i];
    for(j=0;j<*vars;j++) {
      fscanf(fp,"%d",temp);
      temp++;
    }
  }	
  return matOut;
}

/**************************************************************/
/* reads the info file output by the genepd program-          */
/* contains total number of solutions and number for each var */

statType infoRead (FILE *fp, int vars) {

  int i, j, solns, tempNum;
  double tempDbl;
  char line[LINESIZE] = "init";
  char tempStr[10];
  statType statOut;

  /* initialize variables */
  initStat(&statOut);

  /* look for var tag to get matrix itself */
  while ( strncmp( line, "var", 3) != 0 ) {
    fscanf(fp, "%s", line);
  }
  fscanf(fp, "%s", tempStr);    
  for (i=0; i<vars; i++) {
    fscanf(fp, "%d", &tempNum);
    fscanf(fp, "%d", &solns);
    if (solns == 1) statOut.oneSolns = statOut.oneSolns + 1.0;
  }

  /* look for total_solns tag */
  while ( strncmp( line, "total_solns", 11) != 0 ) {
    fscanf(fp, "%s", line);
  }  
  fscanf(fp, "%lf", &tempDbl);
  statOut.numSolns = (double) tempDbl;

  /* look for user_time tag to get elapsed time */
  while ( strncmp( line, "user_time", 9) != 0 ) {
    fscanf(fp, "%s", line);
  }  
  fscanf(fp, "%lf", &tempDbl);
  statOut.time = tempDbl;

  return statOut;

}

/**************************************************************/
/* tests to see if this predicted network contains any cycles */
/* returns true if it does                                    */

BOOL cyclicNet(char *simFile, char *predFile) {

  char predFileDel[32], simFileDel[32], command[32];
  FILE *fp;

  /* run nw_del on predicted network file*/
  sprintf(command, "nw_del %s %s.del > /dev/null", predFile, predFile); 
  system(command); 

  /* try to open .del file.  If fails, there must have been a cycle */
  sprintf(predFileDel, "%s.del", predFile);
  fp = fopen(predFileDel, "r");
  if (fp == NULL) {
    return TRUE;
  }
  else {
  /* made it to here, so no cycle in this network */
    fclose(fp);
    return FALSE;
  }
}

/**************************************************************/
/* compares two matrices by some appropriate metrics- for now */
/* score is number of shared edges in graph                   */

void nwCompare(statType *stats, int **mat1, int **mat2, 
	      int num_vars, int iter, int simEdges) {
  
  int row, col;
  int **matOut;

  /* allocate space for output matrix */
  matOut = calloc(num_vars, sizeof(int *));
  for (row=0; row<num_vars; row++) 
    matOut[row] = calloc(num_vars, sizeof(int)); 

  /* get number of edges in sim and predicted graph */
  stats->predEdges = mat_and(num_vars, mat2, mat2, matOut);

  /* compute shared edges */
  stats->shared = mat_and(num_vars, mat1, mat2, matOut);      

  /* determine if actual graph found */
  if (simEdges == stats->shared) stats->simFound = 1.0;
  else stats->simFound = 0.0;

  /* deallocate space for output matrix */
  for (row=0; row<num_vars; row++) free(matOut[row]);
  free(matOut);

}


/************************************************************/

int main (int argc, char *argv[]) {

  int i, j, nw_num, pnw_num;
  int num_nodes, cycles, simEdges;
  double percent;
  statType thisStat, avgStat, infoStat, globalAvg;
  statType meanStat, stdevStat, stderrStat;
  int **simGraph, **predGraph;
  FILE *simFilePtr, *predFilePtr, *infoFilePtr;
  char *fileBase, simFile[32], predFile[32], infoFile[32];
  char num_string[32], command[32], fndText[3] = "no";
  BOOL flag1, flag2;
  statType *statVector;
 
  /* initialize variables */  
  fileBase = NULL;
  simFilePtr = NULL;
  predFilePtr = NULL;
  debug = FALSE;
  header = TRUE;
  initStat(&thisStat);
  initStat(&avgStat);
  initStat(&globalAvg);
  statVector = NULL;
  nw_num = 0;
  flag1 = TRUE;

  /* parse command line */
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i], "-d") == 0) debug = TRUE;
    else if (strcmp(argv[i], "-h") == 0) header = FALSE ;
    else if (fileBase == NULL) fileBase = argv[i];
  }
  if (fileBase == NULL) die (1); 

  /* print header */
  if (header == TRUE) {
    fprintf(OUT, "                                      Shard ");
    fprintf(OUT, "Shard Found Vars   Num    User\n");

    fprintf(OUT, "                    Num    Sim  Pred  Edges ");
    fprintf(OUT, "  /    Sim  w/ 1  Cycles  Time\n");

    fprintf(OUT, "Filename           Solns  Edges Edges (avg) ");
    fprintf(OUT, " Pred Soln? Soln  Removd (sec)\n");

    fprintf(OUT, "---------------- -------- ----- ----- ----- ");
    fprintf(OUT, "----- ----- ----- ------ -----\n");
  } 

  while (flag1 == TRUE) {
    /* construct simulated network filename */
    strcpy(simFile, fileBase);
    sprintf(num_string, "%d.nw", nw_num);
    strcat(simFile, num_string);
    /* construct info filename */
    strcpy(infoFile, fileBase);
    sprintf(num_string, "%d.pnw.info", nw_num);
    strcat(infoFile, num_string);
    /* attempt to read sim network file */
    if (debug == TRUE) {
      fprintf(stderr, "\nReading simulated net: %s\n", simFile);
      fprintf(stderr, "Reading info file: %s\n", infoFile);
    }
    simFilePtr = fopen(simFile, "r");
    if (simFilePtr != NULL) {
      /* input adjacency matrix from simulated network and calculate */
      /* number of simulated edges now */
      simGraph = nwRead(simFilePtr, &num_nodes);
      simEdges = mat_and(num_nodes, simGraph, simGraph, simGraph);
      fclose(simFilePtr);
      /* input info file for predicted networks */
      /* infoStat gets total num of solns and number of vars with 1 soln */
      /* all other fields initialized to zero as part of infoRead */
      infoFilePtr = fopen(infoFile, "r");
      if (infoFilePtr == NULL) die (2);
      infoStat = infoRead(infoFilePtr, num_nodes);
      fclose(infoFilePtr);
      /* initialize some values for upcoming inner loop */
      initStat(&avgStat);
      cycles = 0;
      pnw_num = 0;    
      flag2 = TRUE;
      while (flag2 == TRUE) {
	/* construct predicted network filename */
	strcpy(predFile, fileBase);
	sprintf(num_string, "%d.pnw%d", nw_num, pnw_num);
	strcat(predFile, num_string);
	/* attempt to read file from disk */
	if (debug == TRUE) 
	  fprintf(stderr, "Reading predicted net: %s\n", predFile);
	predFilePtr = fopen(predFile, "r");
	if (predFilePtr != NULL) {
	  /* test if pred. network is cyclic */
	  if (debug == TRUE) fprintf(stderr, "   Testing for cycles: ");
	  if (cyclicNet(simFile, predFile) == TRUE) {
	    if (debug == TRUE) fprintf(stderr, "cycle found\n");
	    cycles += 1;
	  }
	  else {                   /* if pred. network not cyclic */
	    if (debug == TRUE) fprintf(stderr, "non cyclic\n");
	    /* input adjacency matrix from predicted network */
	    predGraph = nwRead(predFilePtr, &num_nodes);
	    /* compare two matrices */
	    initStat(&thisStat);
	    nwCompare(&thisStat, simGraph, predGraph, 
		      num_nodes, pnw_num, simEdges);
	    /* compute averages */
	    accumStat(thisStat, &avgStat);	  
	    /* free memory for predGraph */
	    for (i=0; i<num_nodes; i++) free(predGraph[i]);
	    free(predGraph);
	  }
	  /* advance to next pred net for same sim net */
	  fclose(predFilePtr);
	  pnw_num++;
	}
	/* if file not found, end loop */
	else {
	  flag2 = FALSE;
	  if (debug == TRUE) 
	    fprintf(stderr, "Predicted net %s not found\n", predFile);
	}
      } /* end inner while loop */
      /* output total statistics for this simulated network */
      if (avgStat.simFound >= 1) sprintf(fndText, "yes");
      else sprintf(fndText, "no");
      /* set number of networks to total number - the ones with cycles */
      if (pnw_num > cycles) divStat(&avgStat, (double) (pnw_num - cycles) );
      avgStat.simEdges = simEdges;
      avgStat.numSolns = infoStat.numSolns;
      avgStat.oneSolns = infoStat.oneSolns;
      avgStat.time = infoStat.time;
      avgStat.cycles = (double) cycles;
      if (avgStat.predEdges > 0)
	avgStat.percent = 100.0 * avgStat.shared / avgStat.predEdges;
      fprintf(OUT, 
	      "%16s %6.2e %5.0f %5.0f %5.1f %4.0f%% %5s %5.0f %6.0f %5.1f\n",
	      simFile, 
	      avgStat.numSolns, avgStat.simEdges, avgStat.predEdges, 
	      avgStat.shared, avgStat.percent, fndText, avgStat.oneSolns,
	      avgStat.cycles, avgStat.time);
      /* compute global averages */
      statVector = realloc(statVector, (nw_num+1) * sizeof(statType));
      initStat(&statVector[nw_num]);
      copyStat(avgStat, &statVector[nw_num]);
      accumStat(avgStat, &globalAvg);
      /* clear avgStat, free memory, and advance to next simulated net */
      initStat(&avgStat);
      for (i=0; i<num_nodes; i++) free(simGraph[i]);
      free(simGraph);
      nw_num++;
    } /* end if */
    /* otherwise, no file found so end */
    else {
      flag1 = FALSE;
      if (debug == TRUE) 
	fprintf(stderr, "Simulated net %s not found\n", simFile);      
    }
  } /* end outer while loop */

  /* if no files read in, exit with error */
  if (nw_num == 0) die (2);

  /* output global statistics */
  computeStat(statVector, nw_num, &meanStat, &stdevStat, &stderrStat);
  fprintf(OUT, "----------------\n");
  fprintf(OUT, "    SAMPLE MEAN: %6.2e %5.1f %5.1f %5.1f %4.0f%% ",
	  meanStat.numSolns, meanStat.simEdges, meanStat.predEdges, 
	  meanStat.shared, meanStat.percent);
  fprintf(OUT, "%11.1f %6.1f %5.1f\n", 
	  meanStat.oneSolns, meanStat.cycles, meanStat.time);
  fprintf(OUT, "   SAMPLE STDEV: %6.2e %5.1f %5.1f %5.1f %4.0f%% ",
	  stdevStat.numSolns, stdevStat.simEdges, stdevStat.predEdges, 
	  stdevStat.shared, stdevStat.percent);
  fprintf(OUT, "%11.1f %6.1f %5.1f\n", 
	  stdevStat.oneSolns, stdevStat.cycles, stdevStat.time); 
  fprintf(OUT, " STANDARD ERROR: %6.2e %5.1f %5.1f %5.1f %4.0f%% ", 
	  stderrStat.numSolns, stderrStat.simEdges, stderrStat.predEdges, 
	  stderrStat.shared, stderrStat.percent);
  fprintf(OUT, "%11.1f %6.1f %5.1f\n", 
	  stderrStat.oneSolns, stderrStat.cycles, stderrStat.time);

  /* free memory and exit program */
  free(statVector);

}


