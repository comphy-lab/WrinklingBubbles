/**
 * @file wrinkling_bub_axi_v1.c
 * @author Saumili Jana (jsaumili@gmail.com)
 * @date 11-07-2025
 * Newtonian cases -with thin liquid film interface
 * 
 * Last update: Oct 28, 2024, Vatsal
 * changelog: fixed the initial condition. 
 * 
 * Last update: Mar 6, 2025, Saumili
 * Changelog: fixed boundary condition
 * 
 * Last update: Mar 15, 2025, Saumili
 * Changelog: updated pressure boundary conditions
 * 
 * Last update: Mar 31, 2025, Saumili
 *  Chhangelog: increased no of iterations for convergence
 * 
 *  * Last update: Apr 4, 2025, Saumili
 *  Chhangelog: corrected pressure initial condition 
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

/*Id1 :indicates the liquid film  media formimg the bubble
Id2: indicates the surrounding gas/fluid(Newtonian)
*/
#define Rho21 (1e-3)
//#define Mu21 (1e-2)
//Calculations

#define Xcent (0.2) //keep same as height of liquid layers
#define Ycent (0.0)

#define R2circle(x,y) (sq(x - Xcent) + sq(y - Ycent))

//Boundary conditions
//velocity //x-axis axisymmetric
u.t[left] = dirichlet(0.0);//wall
u.n[left] = dirichlet(0.0);//wall
f[left] = dirichlet(1.0);// this sets the contact angle to 90 degrees.

u.t[right] = neumann(0.0);//outlow
u.n[right] = neumann(0.0);//outflow
//p[right] = neumann(0.0);//pressure outflow
p[right] = dirichlet(0.0);

u.t[top] = neumann(0.0);//outlow
u.n[top] = neumann(0.0);//outflow
//p[top] = neumann(0.0);//pressure outflow
p[top] = dirichlet(0.0);

//declarations
int MAXlevel;
double tmax, Oh1, Bo, Ldomain, k, h;
float y_p, x_p, x1, x2, x_l, r_fo, r_fi, x_fo, y_fo, x_fi, y_fi, x_ci, x_co, y_ci, y_co, theta_i, theta_o, delta;

int main(int argc, char const *argv[]){
  //assignments
  //NITERMAX = 500; //increased no of iterations for convergence during initial timesteps for some cases
  MAXlevel = 9; //max possible grid res
  tmax = 1.0;
  Ldomain = 2.4;
  

  Bo = atof(argv[1]); //gravity
  Oh1 = atof(argv[2]);//liq film Oh

  k = atof(argv[3]); //curvature R/h

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


//Initial condition// 
event init(t = 0){
  if(!restore (file = "dump")){
    
  
    x_l = Xcent; //height of liquid layer
    h = 1/k;
    y_p = 0.1; 
    x1 = sqrt(sq(1.0-h)-sq(y_p))+x_l;
    x2 = sqrt(1-sq(y_p))+x_l;
    x_p = (x1+x2)/2;

    delta = 0.01;
    //outer fillet
    r_fo = 0.2; //fillet radius
    x_fo = x_l + r_fo;//outer fillet centre x coordinate
    y_fo = sqrt(sq(1+r_fo)-sq(x_fo-x_l)); //outer fillet centre y coordinate
    theta_o = atan(r_fo/y_fo);
    x_co = x_l + sin(theta_o); //contact point-x coordinate
    y_co = cos(theta_o);//contact point-y coordinate
    //inner fillet
    r_fi = 0.0;//fillet radius
    x_fi = x_l + r_fi;//inner fillet centre x coordinate
    y_fi = sqrt(sq(1-h-r_fi)-sq(x_fi-x_l));//inner fillet centre y coordinate
    theta_i = atan(r_fi/y_fi);
    x_ci = x_l+ (1-h)*sin(theta_i);//contact point-x coordinate
    y_ci = (1-h)*cos(theta_i);//contact point-y coordinate


    refine((R2circle(x,y) < 1.05) && (R2circle(x,y) > sq(0.98-h)) && (level < MAXlevel));//bubble shell
    refine((x < (x_l+0.1)) && ((x > x_l-0.1))&&(level < MAXlevel));//liq film

    vertex scalar phi[];

    /*foreach_vertex(){
      if (x<=x_l){
        phi[] = (x_l - x);
      }
     else if ((x>x_l)&&(x<=x_c)){
      if ((y<((y_ci+y_co)/2)) && (y>y_fi)){
        phi [] = (sq(x-x_fi) + sq(y-y_fi) - sq(r_fi)); //inner side fillet
      }
      else if ((y>((y_ci+y_co)/2)) && (y<y_fo)){
        phi [] = (sq(x-x_fo) + sq(y-y_fo) - sq(r_fo)); //outer side fillet
      }
     }
      else {
        if (y >= y_p) {
          // Upper part - spherical rim
          double r = sqrt(sq(x-x_l) + sq(y));
          double shell = min(1. - r, (r - (1. - h)));//check
          phi[] = shell;
        } 
        else {
          // Lower part - half circle
          phi[] = (h/2 - sqrt(sq(x - x_p) + sq(y - y_p)));
        }
      }
    }*/

    foreach_vertex(){
      if (x<x_l){
        phi[] = (x_l - x);
      }
      else if ((x>=x_l)&&(x<=x_ci)&&(y<(1-h/2)) && (y>=y_fi)){
      //if ((y<(1-h/2)) && (y>y_fi)){
        phi [] = (sq(x-x_fi) + sq(y-y_fi) - sq(r_fi)); //inner side fillet
      }
      else if ((x>=x_l)&&(x<=x_co)&&(y>(1-h/2)) && (y<=y_fo)){
        phi [] = (sq(x-x_fo) + sq(y-y_fo) - sq(r_fo)); //outer side fillet
      //}
      }
      else {
        if (y >= y_p) {
          // Upper part - spherical rim
          double r = sqrt(sq(x-x_l) + sq(y));
          double shell = min(1. - r, (r - (1. - h)));//check
          phi[] = shell;
        } 
        else {
          // Lower part - half circle
          phi[] = (h/2 - sqrt(sq(x - x_p) + sq(y - y_p)));
        }
      }
    }
    // foreach_vertex() {
    //   if (y >= y_p) {
    //     // Upper part - spherical rim
    //     double r = sqrt(sq(x) + sq(y));
    //     double shell = min(1. - r, (r - (1. - h)));
    //     phi[] = shell; //&& (x-x_l);
    //   } 
    //   else {
    //     // Lower part - half circle
    //     phi[] = (h/2 - sqrt(sq(x - x_p) + sq(y - y_p)));// && (x-x_l);
    //   }
    // }
    fractions (phi, f);
    
    
    
    //pressure initialize
    foreach(){
      //p[] = (R2circle(x,y)<1)?4:0; //initialize pressure
      if(x<x_l){
        p[] = 2;
      }
      //else if ()
      else{
        if ((R2circle(x,y)<sq(1.0-h)))
        {  
          p[] = 4;
        }
        else if ((R2circle(x,y)<=1)&&((R2circle(x,y)>=sq(1.0-h))))
        {
          p[] = 2;
        }
        else
        {
          p[] = 0;
        }
      }

      u.x[] = 0;
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
      fprintf (ferr, "y_ci %g, y_co %g\n", y_fi, y_fo);//debug
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