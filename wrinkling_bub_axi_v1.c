/**
 * @file wrinkling_bub_axi_v1.c
 * @author Saumili Jana (jsaumili@gmail.com)
 * @date 18-10-2024
 * Newtonuian cases
*/

//f: 1 is liq, 0 is gas phase
#include "axi.h" //remove for 3d case
#include "navier-stokes/centered.h"
#define FILTERED // Smear density and viscosity jumps
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
#define tlog (0.001) //data logging interval

/*Id1 :indicates the liquid film  media formimg the bubble
Id2: indicates the surrounding gas/fluid(Newtonian)
*/
#define Rho21 (10e-3)
#define Mu21 (10e-2)
//Calculations
#define Xcent (0.0)
#define Ycent (0.0)

#define R2circle(x,y) (sq(x - Xcent) + sq(y - Ycent))

//Boundary conditions
//velocity //x-axis axisymmetric
u.t[left] = dirichlet(0.0);
u.n[left] = dirichlet(0.0);
//contact angle? pessure inside bubble film

//declarations
int MAXlevel;
double tmax, Oh1, Bo, Ldomain, k, h;

int main(int argc, char const *argv[]){
    //assignments
  MAXlevel = 10; //max possible grid res
  tmax = 3.0;
  Ldomain = 1.2;

  Bo = 0; //gravity
  Oh1 = 10;//liq film Oh

  k = 10; //curvature R/h

  fprintf(ferr, "Level %d, tmax %g, Bo %g, Oh1 %3.2e, Lo %g\n", MAXlevel, tmax, Bo, Oh1, Ldomain);

  L0=Ldomain;
  X0=0.; Y0=0.;
  init_grid (1 << (4));
  char comm[80];
  sprintf (comm, "mkdir -p intermediate");
  system(comm);

  rho1 = 1.0; 
  rho2 = Rho21;
  f.sigma = 1;//coeff of surface tension
  mu1 = Oh1;
  mu2 = Mu21*Oh1;
  G.x = -Bo; //gravity
  run();
}

//Initial condition// 
event init(t = 0){
  if(!restore (file = "dump")){
    float y_p, x_p, x1, x2;
    h = 1/k;
    y_p = 0.1;
    x1 = sqrt(sq(1.0-h)-sq(y_p));
    x2 = sqrt(1-sq(y_p));
    x_p = (x1+x2)/2;


    refine((R2circle(x,y) < 1.05) && (level < MAXlevel));
    foreach (){
      if ((((y >= y_p)&&(R2circle(x,y)-sq(1.-h))>0)&&(R2circle(x,y)-1.<0))||((y<y_p)&&(sq(x-x_p)+sq(y-y_p)-sq(h/2)<0))){
        f[] = 1;
      }
      else{
        f[] = 0;
      }
    }
   /* foreach (){
      if (((R2circle(x,y)-sq(1.-h))>0)&&(R2circle(x,y)-1.<0)){
        f[] = 1;
      }
      else{
        f[] = 0;
      }
    }*/
    f.prolongation = refine_bilinear;
    boundary((scalar *){f});
    //fraction (f, (1. - R2circle(x,y)) && (R2circle(x,y)-sq(1.0-h)));
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
   p.nodump = false; // dump pressure also
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