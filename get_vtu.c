#include "grid/octree.h"//3d grid
#include "navier-stokes/centered.h"
#include "fractions.h"
#include "output_vtu_foreach.h"

scalar f[];
char filename[80];

int main(int a, char const *arguments[])
{
  sprintf (filename, "%s", arguments[1]);

  restore (file = filename);
  //Boundary conditions
  //velocity
  u.t[left] = dirichlet(0.0);
  u.n[left] = dirichlet(0.0);
  u.r[left] = dirichlet(0.0);
  f.prolongation = fraction_refine;
  boundary(all);
  output_vtu((scalar *) {f, p}, (vector *) {u}, filename);


}

