/* Title: tracking the tip displacement and velocity-v2
# Author: Saumili Jana
# jsaumili@gmail.com
# Date: 24-07-2025

 * Last update: Aug 28, 2025, Saumili
 * Changelog: added tracking angular position of tip
*/
/* Nomenclature:
*(x1,y1): topmost point of the liquid film
*(x2,y2): radially innermost point of the liquid film
*(x3, y3): 
*(x_tip, y_tip): coordinates of the film tip
*phi : angle made by the tip with the x-axis/vertical axis
*/


//f: 1 is liq, 0 is gas phase
#include "axi.h" //remove for 3d case
#include "navier-stokes/centered.h"
#include "fractions.h"
#include "tag.h"
#include "heights.h"

#define Xcent (0.0)
#define Ycent (0.0)

scalar f[], * interfaces = {f};
double vel;
char filename[80], NameOutput[80];//inut file and file with output respetively
float Oh, k;
bool flag = 0; //flag is 1 when (x_tip,y_tip) is assigned - to avoid multiple assignment
bool flip = 0;
char class = 'R';
int main(int a, char const *arguments[]){
  sprintf(filename, "%s", arguments[1]);
  sprintf(NameOutput, "%s", arguments[2]);
  Oh = atof(arguments[3]);
  k = atof(arguments[4]);

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
  double yMax = -HUGE;
  double x_tip = 0;
  double y_tip = 0;
  double f_thresh = threshold;

  double x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
  double phi;


  foreach(){
    if ((f[] > f_thresh) && (f[] < 1. - f_thresh) && (d[] == MainPhase)){
      coord n1 = facet_normal (point, f, s);
      double alpha1 = plane_alpha (f[], n1);
      coord segment1[2];
      if (facets (n1, alpha1, segment1) == 2){
        double xp = x + (segment1[0].x+segment1[1].x)*Delta/2.;
        double yp = y + (segment1[0].y+segment1[1].y)*Delta/2.;
        //fprintf(stderr, "x1 = %g, yMin = %g\n", x1, yMin); //debug
        if (xp>xMax){//check max film height 
          xMax = xp;
          x1 = xp;
          y1 = yp;        
        }
        if (yp<yMin){//track innermost point-film tip
          yMin = yp;
          y2 = yp;
          x2 = xp;
          //uTip = interpolate(u.x, x_tip, yMin);
          //vTip = interpolate(u.y, x_tip, yMin);
        }
      }
    }
  }
  
  //check whether the film has hit the bottom or not?-end execution if it touched the bottom most point
  if (x2<0.05){
    fprintf(ferr, "film tip reached the bottom");
    return 1;
  }

  if (Oh>1){
    fprintf(ferr, "Collapsing Bubble");
    x_tip = x2;
    y_tip = y2;
    flag = 1;
    class = 'C';
  }

  //track (x3,y3)
  foreach(){
    if ((flag != 1)&&(f[] > f_thresh) && (f[] < 1. - f_thresh) && (d[] == MainPhase)){
      coord n1 = facet_normal (point, f, s);
      double alpha1 = plane_alpha (f[], n1);
      coord segment1[2];
      if (facets (n1, alpha1, segment1) == 2){
        double xp1 = x + (segment1[0].x+segment1[1].x)*Delta/2.;
        double yp1 = y + (segment1[0].y+segment1[1].y)*Delta/2.;
        
        if ((xp1>x2)&&(xp1<x1)&&(yp1>yMax)){
          yMax = yp1;
          x3 = xp1;
          y3 = yp1;
        }
      }
    }
  }

  double r_2c = sqrt((sq(x1-x2)+sq(y1-y2))/2);
  double r_3c = sqrt((sq(x1-x3)+sq(y1-y3))/2);
  double x_t = x1-2*r_3c;
   //max threshold for flip
  double delta = 1e-2;
  double y_tMax = -HUGE;
  double y_tMin = HUGE;
  double y_av, x_c, y_c, r_c, theta, x_b;
  
  if ((flag != 1)&&(x2<x_t)){//flipped film
    flip = 1;
    foreach(){
      if ((flag != 1)&&(f[] > f_thresh) && (f[] < 1. - f_thresh) && (d[] == MainPhase)){
        coord n1 = facet_normal (point, f, s);
        double alpha1 = plane_alpha (f[], n1);
        coord segment1[2];
        if (facets (n1, alpha1, segment1) == 2){
          double xp2 = x + (segment1[0].x+segment1[1].x)*Delta/2.;
          double yp2 = y + (segment1[0].y+segment1[1].y)*Delta/2.;
          
          if ((xp2>=x2-delta)&&(xp2<x2+delta)&&(yp2>y_tMax)){
            y_tMax = yp2;
          }
          if ((xp2>=x2-delta)&&(xp2<x2+delta)&&(yp2<y_tMin)){
            y_tMin = yp2;
          }
        }
      }
    }
    y_av = 0.5*(y_tMin+y_tMax);
    x_c = x3;//centre of the circle formed by (x1,y1) and (x2, y2)
    y_c = y1;
    theta = atan2((x_c-x2),(y_c-y_av));

    x_tip = x_c + r_3c*sin(theta);
    y_tip = y_c + r_3c*cos(theta);
    flag = 1;
  }
  else if (flag != 1){
    y_c = y1;
    r_c = (r_2c<r_3c)?r_2c:r_3c ;
    x_c = (r_2c<r_3c)?x2:x3 ;

    x_b = x_c-1.5*r_c;
    if (x_b<delta){
      fprintf(ferr, "film tip reached the bottom -2");
      return 1;
    }

    foreach(){
      if ((flag != 1)&&(f[] > f_thresh) && (f[] < 1. - f_thresh) && (d[] == MainPhase)){
        coord n1 = facet_normal (point, f, s);
        double alpha1 = plane_alpha (f[], n1);
        coord segment1[2];
        if (facets (n1, alpha1, segment1) == 2){
          double xp2 = x + (segment1[0].x+segment1[1].x)*Delta/2.;
          double yp2 = y + (segment1[0].y+segment1[1].y)*Delta/2.;
          
          if ((xp2>=x_b-delta)&&(xp2<x2+delta)&&(yp2>y_tMax)){
            y_tMax = yp2;
          }
          if ((xp2>=x_b-delta)&&(xp2<x2+delta)&&(yp2<y_tMin)){
            y_tMin = yp2;
          }
        }
      }
    }
    y_av = 0.5*(y_tMin+y_tMax);
    theta = atan2((x_c-x_b),(y_c-y_av));

    x_tip = x_c + r_c*sin(theta);
    y_tip = y_c + r_c*cos(theta);
    flag = 1;
  }
  if (flip ==1){class = 'F';}

  phi = atan2(y_tip, x_tip);


  //fprintf(ferr, "xTip %3.2e, YTip %g\n", x_tip, yMin);
  //return 1;

  FILE *fp;
  fp = fopen (NameOutput, "a");
  restore (file = filename);

  if (t == 0){
    fprintf(ferr, "t x1 y1 x2 y2 x3 y3 x_tip y_tip r_2c r_3c phi class\n");
    fprintf(fp, "t x1 y1 x2 y2 x3 y3 x_tip y_tip r_2c r_3c phi class\n");    
  }
  
  fprintf(ferr, "%6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %c\n",t, x1, y1, x2, y2, x3, y3, x_tip, y_tip, r_2c, r_3c, phi, class);
  fprintf(fp, "%6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %6.5e %c\n", t, x1, y1, x2, y2, x3, y3, x_tip, y_tip, r_2c, r_3c, phi, class);
  fclose(fp);

}