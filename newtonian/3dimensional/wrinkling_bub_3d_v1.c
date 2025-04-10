/**
 * @file wrinkling_bub_axi_v1.c
 * @author Saumili Jana (jsaumili@gmail.com)
 * @date 18-10-2024
 * Newtonian cases
 * 
 * Last update: Oct 30, 2024, Saumili
 * changelog: fixed the initial condition for 3d. 
 * 
 * Last update:Mar16, 2025, Saumili
 * changelog: fixed the bounary condition for 3d, and initialised pressure 
 *
 * Last update:Apr10, 2025, Saumili
 * changelog: fixed the pressure initial condition for 3d
 *
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
//#include "vtk.h"//paraview visualization

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

u.t[right] = neumann(0.0);
u.n[right] = neumann(0.0);
u.r[right] = neumann(0.0);
p[right] = dirichlet(0.0);

u.t[top] = neumann(0.0);//outlow
u.n[top] = neumann(0.0);//outflow
u.r[top] = neumann(0.0);
p[top] = dirichlet(0.0);

//pressure inside bubble film??

//declarations
int MAXlevel;
double tmax, Oh1, Bo, Ldomain, k, h;

int main(int argc, char const *argv[]){
    //assignments
  MAXlevel = 8; //max possible grid res
  tmax = 1.0;
  Ldomain = 2;

  Bo = atof(argv[1]); //gravity
  Oh1 = atof(argv[2]);//liq film Oh

  k = atof(argv[3]); //curvature R/h


  fprintf(ferr, "Level %d, tmax %g, Bo %g, Oh1 %3.2e, Lo %g\n", MAXlevel, tmax, Bo, Oh1, Ldomain);

  L0=Ldomain;
  X0=0.; Y0=0.;Z0 = 0;
  init_grid (1 << (4));
  char comm[80];
  sprintf (comm, "mkdir -p intermediate");
  system(comm);

  /*char comm_vtk[80];
  sprintf (comm_vtk, "mkdir -p intermediate_vtk");//for dumping vtk files//comment out when not using
  system(comm_vtk);*/

  rho1 = 1.0; rho2 = Rho21;
  f.sigma = 1;//coeff of surface tension
  mu1 = Oh1; mu2 = 1e-4;// mu2 = Mu21*Oh1;
  G.x = -Bo; //gravity
  run();
}

//Initial condition// 
event init(t = 0){
  if(!restore (file = "dump")){
    double d_h, x_p, y_p, z_p, x1, x2, theta;
    h = 1/k;
    d_h = 0.1;//avg distance of hole//sqrt(y_p^+z_p^2)
    x1 = sqrt(sq(1.0-h)-sq(d_h));
    x2 = sqrt(1-sq(d_h));
    x_p = (x1+x2)/2;

    //refine((R3sphere(x,y,z) < 1.05) && (R3sphere(x,y,z)>sq(1.-h-0.05)) && (level < MAXlevel));
    refine((R3sphere(x,y,z) < 1.05) && (R3sphere(x,y,z) >= sq(0.98-h)) && (level < MAXlevel));
    
    vertex scalar phi[];
    foreach_vertex(reduction(+:theta), reduction(+:y_p), reduction(+:z_p)){
      theta = atan(z/y);
      y_p = d_h*cos(theta);
      z_p = d_h*sin(theta);

      if(sq(y)+sq(z) >= sq(d_h)){
        double r = sqrt(sq(x) + sq(y) + sq(z));
        double shell = min(1. - r, (r - (1. - h)));
        phi[] = shell;
      }
     else {
      phi[] = (sq(h/2.)-(sq(x-x_p)+sq(y-y_p)+sq(z-z_p)));
      }
      
    }
    fractions (phi,f);  
    //f.prolongation = refine_bilinear;
    //boundary((scalar *){f});

    //pressure initialize
    foreach(){
      //p[] = (R3sphere(x,y,z)<1)?4:0; //initialize pressure
      if ((R3sphere(x,y,z)<sq(1.0-h)))
      {  p[] = 4;
      }
      else if ((R3sphere(x,y,z)<=1)&&((R3sphere(x,y,z)>=sq(1.0-h))))
      {
         p[] = 2;
      }
      else
      {
        p[] = 0;
      }


      u.x[] = 0;//initialize velocity
      u.y[] = 0;
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
}

//Outpts
//static
event writingFiles (t = 0, t += tsnap; t <= tmax) {
  p.nodump = false; // dump pressure also
  dump (file = "dump");
  char nameOut[80];
  sprintf (nameOut, "intermediate/snapshot-%5.4f", t);
  dump (file = nameOut);

  //vtk outputs, comment out block when not needed
  /*char nameOut_vtk[80];
  sprintf (nameOut_vtk, "intermediate_vtk/snapshot-%5.4f", t);
  char tail[8] = ".vtk";
  strcat(nameOut_vtk,tail);
  FILE *fp=fopen(nameOut_vtk,"w");
  bool linear=true;
  int n=N;
  output_vtk(all,n,fp,linear);
  fclose(fp);*///vtkend
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