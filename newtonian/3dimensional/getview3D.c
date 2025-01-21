// making videos

#include "grid/octree.h"
#include "navier-stokes/centered.h"
#include "view.h"
#include "draw.h"

scalar f[];
char filename[80], Imagename[80], tstep[20];
/**
 * Main function to render and save a 3D view based on given input files.
 *
 * This function takes two command line arguments: the first is the filename
 * for restoring data, and the second is the image name for saving the 
 * rendered output. It sets up the viewing parameters using pre-defined
 * quaternion and field-of-view settings, applies various transformations
 * including mirroring in different directions, and visualizes the field
 * using draw_vof, squares, and cells functions. Finally, it saves the
 * rendered image to the specified file.
 *
 * @param a The number of command line arguments.
 * @param arguments Array of command line arguments, where arguments[1] is
 *                  the input filename and arguments[2] is the output image
 *                  name.
 */
int main(int a, char const *arguments[])
{
  sprintf (filename, "%s", arguments[1]);
  sprintf (Imagename, "%s",arguments[2]);
  sprintf (tstep, "%s",arguments[3]);
  restore (file = filename);

  
view (quat = {0.055, 0.715, -0.033, 0.696},
      fov = 30, near = 0.01, far = 1000,
      tx = 0.069, ty = 0.036, tz = -4.607,
      width = 756, height = 589,
      bg = {1,1,1});

  draw_vof (c = "f", edges=false, fc = {0.91, 0.41, 0.17});
  squares(color = "0.0", min=-1.0, max=1.0, n = {0, 1, 0}, alpha=-0.90, map = cool_warm);
  cells(n = {0, 1, 0}, lc = {0., 0., 0.}, alpha=-0.90, lw = 1);
  
  
  // Mirror in negative z direction
  begin_mirror(n={0,0,-1}, alpha=0.);
  draw_vof (c = "f", edges=false, fc = {0.91, 0.41, 0.17});
  squares(color = "0.0", min=-1.0, max=1.0, n = {0, 1, 0}, alpha=-0.90, map = cool_warm);
  cells(n = {0, 1, 0}, lc = {0., 0., 0.}, alpha=-0.90, lw = 1);
  
  // Mirror in negative y direction
  begin_mirror(n={0,-1,0}, alpha=0.);
  draw_vof (c = "f", edges=false, fc = {0.91, 0.41, 0.17});
  squares(color = "0.0", min=-1.0, max=1.0, n = {0, 1, 0}, alpha=-0.90, map = cool_warm);
  cells(n = {0, 1, 0}, lc = {0., 0., 0.}, alpha=-0.90, lw = 1);
  end_mirror();
  end_mirror();
  
 // Mirror in positive y direction
  begin_mirror(n={0,1,0}, alpha=0.);
  draw_vof (c = "f", edges=false, fc = {0.91, 0.41, 0.17});
  squares(color = "0.0", min=-1.0, max=1.0, n = {0, 1, 0}, alpha=-0.90, map = cool_warm);
  cells(n = {0, 1, 0}, lc = {0., 0., 0.}, alpha=-0.90, lw = 1);
  end_mirror();
  //annotate time
  draw_string ("t =", pos = 1, size = 20, lc = {255, 255, 255}, lw = 4);
  char tp[25] = "    ";
  strcat(tp, tstep);
  draw_string (tp, pos = 1, size = 20, lc = {255, 255, 255}, lw = 4);
  
  save (Imagename);
}
