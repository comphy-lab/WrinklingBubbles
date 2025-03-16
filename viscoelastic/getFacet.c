/* Title: Getting Facets
# Author: Vatsal Sanjay
# vatsalsanjay@gmail.com
# Physics of Fluids
*/
#include "navier-stokes/centered.h"
#include "fractions.h"

scalar f[];
char filename[80];
int main(int a, char const *arguments[])
{
  if (a < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", arguments[0]);
    return 1;
  }
  sprintf (filename, "%s", arguments[1]);
  restore (file = filename);
  f.prolongation = fraction_refine;
  FILE * fp = ferr;
  output_facets(f, fp);
  fflush (fp);
  fclose (fp);
}
