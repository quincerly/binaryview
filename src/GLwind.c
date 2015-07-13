#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <binary.h>
#include "GLlobe.h"

void init_wind(windstruc *wind, double x, double y, double z)
{

  wind->x=x;
  wind->y=y;
  wind->z=z;

}

void draw_wind(windstruc *wind, double dx,double dy,double dz)
{

  if (wind->r>0.) {

    glPushMatrix();
    glTranslated(wind->x,wind->y,wind->z);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    
    draw_sphere(wind->r,wind->red,wind->green,wind->blue,
		0.,1.,
		wind->op, 0.,
		1.,0.,
		dx,dy,dz);
  
    glPopMatrix();
  }

}

double opwindfunc(double r)
{
  return(exp(-r*r));
  //  return(1.);
  //  return(exp(-r));
}
