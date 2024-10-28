



/**
 * @file wrinkling_bub_axi_v1.c
 * @author Saumili Jana (jsaumili@gmail.com)
 * @date 18-10-2024
 * Newtonian cases
 * 
 * Last update: Oct 28, 2024, Vatsal
 * changelog: fixed the initial condition. 
*/

//f: 1 is liq, 0 is gas phase
#include "grid/octree.h"//3d grid
#include "navier-stokes/centered.h"
#define FILTERED // Smear density and viscosity jumps
#include "two-phase.h"
#include "navier-stokes/conserving.h"
// #include "log-conform-ViscoElastic_v6.h" // VE part
#include "tension.h"
#include "reduced.h"//gravity

// #include "output_vtu_foreach.h"//paraview visualization

// error tolerances //for AMR
#define fErr (1e-3)
#define VelErr (1e-3)
#define KErr (1e-3)
#define AErr (1e-3)
#define MINlevel 2 

//time-intervals for saving
#define tsnap (0.01)  //snapshot saving interval
#define tlog (0.001) //data logging interval

/*Id1 :indicates the liquid film  media formimg the bubble
Id2: indicates the surrounding gas/fluid(Newtonian)
*/
#define Rho21 (1e-3)
#define Mu21 (1e-2)
//Calculations
#define Xcent (0.0)
#define Ycent (0.0)
#define Zcent (0.0)

#define R3sphere(x,y,z) (sq(x - Xcent) + sq(y - Ycent) + sq(z - Zcent))

//Boundary conditions
//velocity //x-axis axisymmetric
u.t[left] = dirichlet(0.0);
u.n[left] = dirichlet(0.0);
u.r[left] = dirichlet(0.0);
f[left] = neumann(0.0); // this sets the contact angle to 90 degrees.

//declarations
int MAXlevel;
double tmax, Oh1, Bo, Ldomain, k, h;

int main(int argc, char const *argv[]){
  //assignments
  MAXlevel = 7; //max possible grid res
  tmax = 1.0;
  Ldomain = 1.2;

  Bo = 1e-3; //gravity
  Oh1 = 1.0;//liq film Oh

  k = 10; //curvature R/h

  fprintf(ferr, "Level %d, tmax %g, Bo %g, Oh1 %3.2e, Lo %g\n", MAXlevel, tmax, Bo, Oh1, Ldomain);

  L0=Ldomain;
  X0=0.; Y0=0.;Z0 = 0;
  init_grid (1 << (4));
  char comm[80];
  sprintf (comm, "mkdir -p intermediate");
  system(comm);

  char comm_vtu[80];
  sprintf (comm_vtu, "mkdir -p intermediate_vtu");//for dumping vtu files//comment out when not using
  system(comm_vtu);

  rho1 = 1.0; rho2 = Rho21;
  f.sigma = 1;//coeff of surface tension
  mu1 = Oh1; mu2 = Mu21*Oh1;
  G.x = -Bo; //gravity
  run();
}

//Initial condition// 
event init(t = 0) {
  if (!restore (file = "dump")) {
    float y_p, x_p, x1, x2;
    h = 1/k;
    y_p = 0.1;
    x1 = sqrt(sq(1.0-h)-sq(y_p));
    x2 = sqrt(1-sq(y_p));
    x_p = (x1+x2)/2;


    
    refine((R3sphere(x,y,z) < 1.05) && (level < MAXlevel));
    
    vertex scalar phi[];
    foreach_vertex() {
      if (y >= y_p) {
        // Upper part - spherical shell
        double r = sqrt(sq(x) + sq(y) + sq(z));
        double shell = min(1. - r, (r - (1. - h)));
        phi[] = shell;
      }
      else {
        // Lower part - toroidal rim
        phi[] = (h/2. - sqrt(sq(y - y_p) + sq(sqrt(sq(x-x_p+1.)+sq(z-x_p+1.))-1)));
      }
    }
    fractions (phi, f);
    dump (file = "dump");
    return 1;
  }
}

//AMR
scalar KAPPA[];
event adapt(i++){
  curvature(f, KAPPA);
  adapt_wavelet ((scalar *){f, u.x, u.y, KAPPA},
    (double[]){fErr, VelErr, VelErr, KErr},
    MAXlevel, MAXlevel-4);
  //unrefine(x>150.0);

}

//Outpts
//static
event writingFiles (t = 0, t += tsnap; t <= tmax) {
  dump (file = "dump");
  char nameOut[80];
  sprintf (nameOut, "intermediate/snapshot-%5.4f", t);
  dump (file = nameOut);

  //vtu outputs, comment out block when not needed
  // char nameOut_vtu[80];
  // sprintf (nameOut_vtu, "intermediate_vtu/snapshot-%5.4f", t);
  // output_vtu((scalar *) {f, p}, (vector *) {u}, nameOut_vtu);
  //vtuend
}

event logWriting (i++) {

  static FILE * fp;
  if (pid() == 0){
    if (i == 0) {
      fprintf (ferr, "i dt t\n");
      fp = fopen ("log", "w");
      fprintf (fp, "Level %d, tmax %g, Oh %3.2e, Bo %2.1e, Lo %g\n", MAXlevel, tmax, Oh1, Bo, Ldomain);
      fprintf (fp, "i dt t\n");
      fprintf (fp, "%d %g %g \n", i, dt, t);
      fclose(fp);
    } else {
      fp = fopen ("log", "a");
      fprintf (fp, "%d %g %g\n", i, dt, t);
      fclose(fp);
    }
    fprintf (ferr, "%d %g %g\n", i, dt, t);
  }

}

