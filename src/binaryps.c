#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <binary.h>
#include <misc.h>
#include <stream.h>
#include <visibility.h>
#include "drawbinary.h"
#include "glps.h"
#include "getpar.h"
#include "initpars.h"
#include "tiffio.h"
#include "writetiff.h"

double scale;
int startx,starty;
int cleartoblack;
int window_width0,window_height0;
int window_width,window_height;
double left_offset=0.,bottom_offset=0.;
//int LEFT_MOUSE_DOWN=0;

/* Postscript related variables */
char epsname[60];
int objectComplexity=2000000;

/* My function protos */
void reshape(int width, int height);
void display(void);
void mouse(int button, int state, int x, int y);

/* Main program */
int main(int argc, char **argv)
{

  FILE *configfile;
  if (argc!=2) {
    printf("Usage : binaryps configfile\n\n");
    exit(0);
  }
  if (!(configfile=fopen(argv[1],"r"))) {
    printf("Couldn't open configuration `%s'.\n",argv[1]);
    exit(0);
  }

  if (initpars(configfile)==0) {
    fclose(configfile);
    exit(0);
  }
  if (getpar(configfile,"epsname",'S',&epsname)==0) {
    fclose(configfile);
    exit(0);
  }
  if (getpar(configfile,"orbphase",'D',&phasedeg)==0) phasedeg=0;
  if (getpar(configfile,"clear",'I',&cleartoblack)==0) cleartoblack=0;
  if (getpar(configfile,"scale",'D',&scale)==0) scale=1.;
  if (getpar(configfile,"width",'I',&window_width0)==0) window_width0=350;
  if (getpar(configfile,"height",'I',&window_height0)==0) window_height0=350;
  fclose(configfile);

  nstreamdraw=nstream;

  /* Limb darkening parameters */
  limba=0.4;
  limbb=0.6;

  window_width=window_width0;
  window_height=window_height0;
  scale*=3.;
  phasedeg*=360.;
  hotspotphase=hotspotphase0;
  
  nthetastream=20;
  nphicomp=5;
  nzcomp=5;
  rcomp=0.005;
  redcomp=0.8;
  greencomp=0.8;
  bluecomp=0.8;

  // Initialize parameters
  binary_init();
  printf("q %g, i %g\n",q,incdeg);
  printf("x1 %5.3f, x2 %5.3f\n",x1,x2);
  printf("xl1 %5.3f, rd2 %5.3f, potl1 %g\n",xl1,rd2,potl1);

  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (window_width, window_height);
  glutCreateWindow (argv[0]);
  glutReshapeFunc (reshape);
  glutDisplayFunc(display);
  glutMouseFunc(mouse);

  // Initialize GL
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);
  glViewport((GLint)((left_offset+0.5*(1.-scale))*window_width),
	     (GLint)((bottom_offset+0.5*(1.-scale))*window_height),
	     (GLint)(scale*window_width),
	     (GLint)(scale*window_height));
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  glOrtho(-3., 3., -3., 3., -10.0, 10.0); 
  //  glScaled(1,(double)window_width/(double)window_height,1.);
  glRotated(-incdeg,1.,0.,0.); 
  glMatrixMode(GL_MODELVIEW);

  // Start it
  glutMainLoop();

  freeall();
  return 0;

}

void mouse(int button, int state, int x, int y)
{

  double xscale,yscale;
  
  xscale=(double)window_width/350.;
  yscale=(double)window_height/350.;

  switch(button) {
  case GLUT_RIGHT_BUTTON:
    outputEPS(objectComplexity, 1, epsname);
    writetiff("binary.tiff", "OpenGL-rendered interacting binary star", 0, 0, window_width,window_height, COMPRESSION_NONE);
    //writetiff("binary.tiff", "OpenGL-rendered interacting binary star", 0, 0, W, H, COMPRESSION_PACKBITS);
    exit(0);
    break;
  case GLUT_LEFT_BUTTON:
    if (state==GLUT_DOWN) {
      startx=x;
      starty=y;
    } 
    else {
      left_offset+=(x-startx)*1./window_width;
      bottom_offset+=(starty-y)*1./window_height;
      //      left_offset+=(int)((x-startx)*xscale);
      //      bottom_offset+=(int)((starty-y)*yscale);
      glViewport((GLint)((left_offset+0.5*(1.-scale/xscale))*window_width),
		 (GLint)((bottom_offset+0.5*(1.-scale/yscale))*window_height),
		 (GLint)(scale/xscale*window_width),
		 (GLint)(scale/yscale*window_height));
      glutPostRedisplay();      
    }
    break;
  }

}

void reshape(int width, int height)
{


  double xscale,yscale;

  left_offset=window_width*left_offset/width;
  bottom_offset=window_height*bottom_offset/height;

  window_width=width;
  window_height=height;

  xscale=(double)window_width/window_width0;
  yscale=(double)window_height/window_height0;

  // Set the new viewport size
  glViewport((GLint)((left_offset+0.5*(1.-scale/xscale))*window_width),
	     (GLint)((bottom_offset+0.5*(1.-scale/yscale))*window_height),
	     (GLint)(scale/xscale*window_width),
	     (GLint)(scale/yscale*window_height));

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-3., 3., -3., 3., -10.0, 10.0);
  //  glOrtho(-3.*xscale, 3.*xscale, -3.*yscale, 3.*yscale, -10.0, 10.0);
  //  glScaled(1,(double)window_width/(double)window_height,1.);
  glRotated(-incdeg,1.,0.,0.);
  glMatrixMode(GL_MODELVIEW);

}


void display(void)
{

  // Clear the RGB buffer and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw it
  render();

}

#include "getpar.c"
#include "drawbinary.c"
#include "initpars.c"
