/* Title: tracking the tip displacement and velocity
# Author: Saumili Jana
# jsaumili@gmail.com
# Date: 04-01-2025
*/

//f: 1 is liq, 0 is gas phase
#include "axi.h" //remove for 3d case
#include "navier-stokes/centered.h"
#include "fractions.h"
#include "tag.h"
#include "heights.h"
#include "reduced.h"//gravity

scalar f[];
double vel;
char filename[80], NameOutput[80]; //inut file and file with output respetively

int main(int a, char const *arguments[]){
  sprintf(filename, "%s", arguments[1]);
  sprintf(NameOutput, "%s", arguments[2]);
  restore(file = filename);

  //fprintf(ferr, "Ohd %3.2e, We %g\n", Ohd, We);
  //return 1;

  //boundary conditions
  //velocity //x-axis axisymmetric
  u.t[left] = dirichlet(0.0);
  u.n[left] = dirichlet(0.0);
  f[left] = neumann(0.0); // this sets the contact angle to 90 degrees.
  f.prolongation = refine_bilinear;
  boundary((scalar *){f, u.x, u.y, p});


  

}