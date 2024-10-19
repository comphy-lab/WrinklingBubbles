/**
 * @file wrinkling_bub_axi_v1.c
 * @author Saumili Jana (vatsalsanjay@gmail.com)
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


//Boundary conditions
//velocity //x-axis axisymmetric
u.t[left] = dirichlet(0.0);
u.n[left] = dirichlet(0.0);
//contact angle?


//declarations
int MAXlevel;
double tmax, Oh1, Oh2, Bo, Ldomain, k;

int main(int argc, char const *argv[]){
    //assignments
MAXlevel = 10; //max possible grid res
tmax = 1.0;
Ldomain = 2;

//Bo = 0; //gravity
Oh1 = 10;//liq film Oh
Oh2 = 1e-4;//surrunding Oh
k = 10; //curvature R/h
L0=Ldomain;
X0=0.; Y0=0.;
init_grid (1 << (4));
char comm[80];
sprintf (comm, "mkdir -p intermediate");
system(comm);

rho1 = 1.0; 
rho2 = Rho21;

//mu1 = Oh1/






    run();
}

//Initial condition


//AMR


//Outpts
//static
