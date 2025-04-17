/* Title: tracking the tip displacement and velocity
# Author: Saumili Jana
# jsaumili@gmail.com
# Date: 17-04-2025
*/

//f: 1 is liq, 0 is gas phase
#include "axi.h" //remove for 3d case
#include "navier-stokes/centered.h"
#include "fractions.h"
#include "tag.h"
#include "heights.h"

scalar f[], * interfaces = {f};
double vel;
char filename[80], NameOutput[80]; //inut file and file with output respetively

int main(int a, char const *arguments[]){
  sprintf(filename, "%s", arguments[1]);
  sprintf(NameOutput, "%s", arguments[2]);

  restore(file = filename);
  boundary((scalar *){f, u.x, u.y, p});

  // tag all liquid parts starts
  scalar d[];
  double threshold = 1e-6;
  foreach(){
    d[] = (f[] > threshold);
  }
  int n = tag (d), size[n];
  for (int i = 0; i < n; i++){
    size[i] = 0;
  }
  foreach_leaf(){
    if (d[] > 0){
      size[((int) d[]) - 1]++;
    }
  }
  int MaxSize = 0;
  int MainPhase = 0;
  for (int i = 0; i < n; i++){
    // fprintf(ferr, "%d %d\n",i, size[i]);
    if (size[i] > MaxSize){
      MaxSize = size[i];
      MainPhase = i+1;
    }
  }
  for (int i = 0; i < n; i++){
    fprintf(ferr, "Tag %d has size %d\n", i+1, size[i]);//debugging
  }//check for tiny strays
  // tag all liquid parts ends
  fprintf(ferr, "successfully tagged liquid region\n");//debugging
  
  face vector s[];
  s.x.i = -1;
  double yMin = HUGE;
  double xMax = -HUGE;
  double uTip = 0, vTip = 0;
  double x_tip = 0;
  double y_tip = 0;
  double f_thresh = 1e-6;

  foreach(){
    if ((f[] > f_thresh) && (f[] < 1. - f_thresh) && (d[] == MainPhase)){
      coord n1 = facet_normal (point, f, s);
      double alpha1 = plane_alpha (f[], n1);
      coord segment1[2];
      if (facets (n1, alpha1, segment1) == 2){
        double x1 = x + (segment1[0].x+segment1[1].x)*Delta/2.;
        double y1 = y + (segment1[0].y+segment1[1].y)*Delta/2.;
        //fprintf(stderr, "x1 = %g, yMin = %g\n", x1, yMin); //debug
        if (y1<yMin){//track innermost point-film tip
          yMin = y1;
          y_tip = y1;
          x_tip = x1;
          uTip = interpolate(u.x, x_tip, yMin);
          vTip = interpolate(u.y, x_tip, yMin);
        }
        if (x1>xMax){
          xMax = x1;    //check max film height     
        }

      }
    }
  }

  if (x_tip<= xMax-0.15){ //if significant distance between x_tip detected and max film height>>film gemetry different
    fprintf(stderr, "xtip = %g, xMax = %g\n", x_tip, xMax); 
    fprintf("detected tip is incorrect due to geometry of the film, significant distance between x_tip and xMax"); 
    foreach(){
      if ((f[] > f_thresh) && (f[] < 1. - f_thresh) && (d[] == MainPhase)){
        coord n1 = facet_normal (point, f, s);
        double alpha1 = plane_alpha (f[], n1);
        coord segment1[2];
        if (facets (n1, alpha1, segment1) == 2){
          double x1 = x + (segment1[0].x+segment1[1].x)*Delta/2.;
          double y1 = y + (segment1[0].y+segment1[1].y)*Delta/2.;
          //fprintf(stderr, "x1 = %g, yMin = %g\n", x1, yMin); debug
          if((y1<yMin+0.075)&&(x1>x_tip)){
            x_tip = x1;
            y_tip = y1;
            uTip = interpolate(u.x, x_tip, y_tip);
            vTip = interpolate(u.y, x_tip, y_tip);
          }

        }
      }
    }
  }
  //fprintf(ferr, "xTip %3.2e, YTip %g\n", x_tip, yMin);
  //return 1;

  //boundary conditions -- these you do not need in the post-processing script. In the older versions, you needed to set the BCs in the post-processing script, not anymore.
  //velocity //x-axis axisymmetric
  // u.t[left] = dirichlet(0.0);
  // u.n[left] = dirichlet(0.0);
  // f[left] = neumann(0.0); // this sets the contact angle to 90 degrees.
  // f.prolongation = refine_bilinear;
  // boundary((scalar *){f, u.x, u.y, p});
  
  FILE *fp;
  fp = fopen (NameOutput, "a");
  restore (file = filename);

  if (t == 0){
    fprintf(ferr, "t x_tip y_tip uTip vTip\n");
    fprintf(fp, "t x_tip y_tip uTip vTip\n");    
  }
  
  fprintf(ferr, "%6.5e %6.5e %6.5e %6.5e %6.5e \n", t, x_tip, y_tip, uTip, vTip);
  fprintf(fp, "%6.5e %6.5e %6.5e %6.5e %6.5e \n", t, x_tip, y_tip, vTip, uTip);
  fclose(fp);

}
