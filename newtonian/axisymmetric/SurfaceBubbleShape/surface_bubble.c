/**
 * @file surface_bubble.c
 * @author Saumili Jana (sjana.comphy@gmail.com)
 * @date 25-08-2025
 * Study the formation and shape of a surface bubble
 * Newtonian cases
 * 
 */

 //f: 1 is liq, 0 is gas phase
#include "axi.h" //remove for 3d case
#include "navier-stokes/centered.h"
#define FILTERED 1// Smear density and viscosity jumps
#include "two-phase.h"
#include "navier-stokes/conserving.h"
// #include "log-conform-ViscoElastic_v6.h" // VE part
#include "tension.h"
#include "reduced.h"//gravity

// error tolerances //for AMR
#define fErr (1e-3)
#define VelErr (1e-3)
#define KErr (1e-3)
#define AErr (1e-3)
#define MINlevel 2 

//time-intervals for saving
#define tsnap (0.01)  //snapshot saving interval

/*Id1 :indicates the liquid media 
Id2: indicates the gas/fluid(Newtonian) in side the bubble
*/
#define Rho21 (1e-3)
//#define Mu21 (1e-2)
//Calculations
#define Xcent (1.5)
#define Ycent (0.0)

#define R2circle(x,y) (sq(x - Xcent) + sq(y - Ycent))

//Boundary conditions
 //x-axis axisymmetric
// u.t[left] = dirichlet(0.0);//wall
// u.n[left] = dirichlet(0.0);//wall
// f[left] = dirichlet(1.0); // bottom wall is always wetting

// u.t[right] = neumann(0.0);//outlow
// u.n[right] = neumann(0.0);//outflow
// //p[right] = neumann(0.0);//pressure outflow
// p[right] = dirichlet(0.0);

// u.t[top] = neumann(0.0);//outlow
// u.n[top] = neumann(0.0);//outflow
// //p[top] = neumann(0.0);//pressure outflow
// //p[top] = dirichlet(0.0);

//declarations
int MAXlevel;
double tmax, Oh1, Bo, Ldomain, d, x_l;

int main(int argc, char const *argv[]){
  //assignments
    MAXlevel = 9; //max possible grid res
    tmax = 10.0;
    Ldomain = 4.8;

    Bo = atof(argv[1]); //gravity
    Oh1 = atof(argv[2]);//liq Oh

    
    // k = atof(argv[3]); //curvature R/d
    d = 1.05; //distance of immersed bubble centre from surface
    x_l = Xcent + d;

    fprintf(ferr, "Level %d, tmax %g, Bo %g, Oh1 %3.2e, Lo %g\n", MAXlevel, tmax, Bo, Oh1, Ldomain);

    L0=Ldomain;
    X0=0.; Y0=0.;
    init_grid (1 << (4));
    char comm[80];
    sprintf (comm, "mkdir -p intermediate");
    system(comm);

    rho1 = 1.0; rho2 = Rho21;
    f.sigma = 1; //coeff of surface tension
    mu1 = Oh1; mu2 = 1e-4;// mu2 = Mu21*Oh1;
    G.x = -Bo; //gravity
    run();
}

event init(t = 0){
  if(!restore (file = "dump")){
    refine((R2circle(x,y) < 1.05) && (level < MAXlevel));//buble interface
    refine((x < (x_l+0.1)) && ((x > x_l-0.1))&&(level < MAXlevel));//liq film interface

    vertex scalar phi[];

    foreach_vertex(){
      if (x>x_l){
        phi[] = (x_l-x);// upper half of domain
      }
      else{
        phi[] = (R2circle(x,y)-1.0); //initialise the bubbble
      }
    }
    fractions(phi, f);

    //pressure and velocity initial conditions
    foreach()
    if (R2circle(x,y)<1){
      p[] = 2;
    }
    else {
      p[] = 0;
    }

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
  p.nodump = false; // dump pressure
  dump (file = "dump");
  char nameOut[80];
  sprintf (nameOut, "intermediate/snapshot-%5.4f", t);
  dump (file = nameOut);
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