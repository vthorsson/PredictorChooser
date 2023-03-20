/* utility functions */
#include "util.h"

/* generic error handling function */
void die (int error_code) {
  
  char usage[1000];

  sprintf(usage, "%s%s%s%s%s", 
	  "  Usage:    genepd <infile> <outfile>\n\n",
	  "Options:    -a        output all solutions                   \n",
	  "            -l <num>  limit to <num> solutions               \n",
	  "            -r <num>  randomly sample solutions <num> times  \n",
	  "            -d        debug mode: print diagnostic output    \n");

  switch (error_code) {

  case 0: 
    fprintf(stderr, "\nMemory allocation failure.\n");
    break;
  case 1: 
    fprintf(stderr, "\nCommand line error.\n\n%s\n\n", usage);
    break;
  case 2: 
    fprintf(stderr, "\nCannot read input file\n");
    break;
  case 3:
    fprintf(stderr, "\nNumber out of range\n");
    break;
  case 5:
    fprintf(stderr, "\nGenerated non-binary value\n");
    break;
  default: 
    fprintf(stderr, "\nUnknown error.\n");  
    break;
  }
  fprintf(stderr, "Aborting program execution\n");
  exit(1);
}

/* timing functions */

void start_time (void) {
  tk.begin_clock = tk.save_clock = clock();
  tk.begin_time = tk.save_time = time(NULL);
}

double prn_time (FILE *fp) {

  char s1[MAXSTRING], s2[MAXSTRING];
  int field_width, n1, n2;
  double clocks_per_second = (double) CLOCKS_PER_SEC, 
    user_time, real_time;

  user_time = ( clock() - tk.save_clock ) / clocks_per_second;
  real_time = difftime( time(NULL), tk.save_time );
  tk.save_clock = clock();
  tk.save_time = time(NULL);

  /* print values found, and do it neatly */
  n1 = sprintf(s1, "%.1f", user_time);
  n2 = sprintf(s2, "%.1f", real_time);
  field_width = (n1 > n2) ? n1 : n2;
  fprintf(fp, "%s%*.1f%s\n%s%*.1f%s\n\n",
	 "user_time ", field_width, user_time, " seconds",
	 "real_time ", field_width, real_time, " seconds");
  return user_time;
}
