#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "GLstarfield.h"

void init_starfield(starfieldstruc *starfield, systemparsstruc *sys)
{
  starfield->x=(double *)malloc(starfield->n*sizeof(double));
  starfield->y=(double *)malloc(starfield->n*sizeof(double));
  starfield->z=(double *)malloc(starfield->n*sizeof(double));
  starfield->r=(double *)malloc(starfield->n*sizeof(double));
  starfield->red=(double *)malloc(starfield->n*sizeof(double));
  starfield->green=(double *)malloc(starfield->n*sizeof(double));
  starfield->blue=(double *)malloc(starfield->n*sizeof(double));

  calc_starfield(starfield,sys);
}

void free_starfield(starfieldstruc *starfield)
{
  free(starfield->x);
  free(starfield->y);
  free(starfield->z);
  free(starfield->r);
  free(starfield->red);
  free(starfield->green);
  free(starfield->blue);
}

/* Generate starfield */
void calc_starfield(starfieldstruc *starfield, systemparsstruc *sys)
{
  int star;
  double startheta,starphi,maxcol;

  for (star=0;star<starfield->n;star++) {
    startheta=(RND-0.5)*0.2*PI*0.5+0.5*PI;
    starphi=(RND-0.5)*PI+PI-sys->incdeg/180.*PI;    
    starfield->x[star]=10.*cos(startheta);
    starfield->y[star]=10.*sin(startheta)*sin(starphi);
    starfield->z[star]=10.*sin(startheta)*cos(starphi);
    starfield->r[star]=RND*0.008;
    maxcol=RND;
    starfield->red[star]=maxcol*(1-0.3*RND*RND);
    starfield->blue[star]=maxcol*(1-0.3*RND*RND);
    starfield->green[star]=(starfield->red[star]+starfield->blue[star])*0.5;
  }
}

void draw_starfield(starfieldstruc *starfield)
{

  int star;

  /* Draw starfield */
  for (star=0;star<starfield->n;star++) {
    glColor3d(starfield->red[star],starfield->green[star],starfield->blue[star]);    
    glPushMatrix();
    glTranslated(starfield->x[star],starfield->y[star],starfield->z[star]);
    glutSolidSphere(starfield->r[star],6.,6.);
    glPopMatrix();
  }
  
}
