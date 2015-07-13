#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "GLsphere.h"

void init_sphere(spherestruc *sphere)
{
  sphere->ntheta=50;
  sphere->nphi=50;

  sphere->x=(double *)malloc(sphere->ntheta*sphere->nphi*sizeof(double));
  sphere->y=(double *)malloc(sphere->ntheta*sphere->nphi*sizeof(double));
  sphere->z=(double *)malloc(sphere->ntheta*sphere->nphi*sizeof(double));

  calc_sphere(sphere);
}
void free_sphere(spherestruc *sphere)
{
  free(sphere->x);
  free(sphere->y);
  free(sphere->z);
}

/* Calculate secondary star Roche lobe surface */
void calc_sphere(spherestruc *sphere)
{

  double theta,dtheta,phi,dphi;
  int itheta,iphi;

  dtheta=PI/(sphere->ntheta-1);
  dphi=2*PI/sphere->nphi;
  for (itheta=0;itheta<sphere->ntheta;itheta++) {
    theta=itheta*dtheta;
    for (iphi=0;iphi<sphere->nphi;iphi++) {
      phi=iphi*dphi;      
      sphere->x[itheta+sphere->ntheta*iphi]=cos(theta);
      sphere->y[itheta+sphere->ntheta*iphi]=cos(phi)*sin(theta);
      sphere->z[itheta+sphere->ntheta*iphi]=sin(phi)*sin(theta);
    }
  }
}

void draw_sphere(spherestruc *sphere,
		 double radius, double red, double green, double blue,
		 double slimba, double slimbb,
		 double op, double oprnd,
		 double r0, double rmin,
		 double dx, double dy, double dz)
{

  int itheta,iphi,index;
  double dtheta,dphi,lfac;
  double x,y,z;
  double cosalpha,sinalpha;
  double phi;
  double sop,pathlength,rclosest;

  dtheta=PI/(sphere->ntheta-1);
  dphi=2*PI/sphere->nphi;
  for (itheta=0;itheta<sphere->ntheta-1;itheta++) {
    glBegin(GL_QUAD_STRIP);
    for (iphi=0;iphi<=sphere->nphi;iphi++) {
      phi=iphi*dphi;
      index=itheta+sphere->ntheta*(iphi%sphere->nphi);
      x=sphere->x[index];
      y=sphere->y[index];
      z=sphere->z[index];
      cosalpha=x*dx+y*dy+z*dz; /* cosine of angle between line of sight and normals */
      lfac=slimba+slimbb*sqrt(cosalpha*cosalpha);                       /* limb darkening factor */
      if (op==0.) glColor3d(lfac*red,lfac*green,lfac*blue);
      else {
	// Pathlength through sphere || line of sight is 2cos(alpha)
	// Radius of closest approach to centre is sin(alpha)
	sinalpha=sqrt(1.-cosalpha*cosalpha);
	pathlength=2.*cosalpha;
	rclosest=sinalpha;
	if (rclosest<rmin) {
	  pathlength=pathlength-2.*sqrt(rmin*rmin-rclosest*rclosest);
	  rclosest=rmin;
	}
	sop=0.5*op*pathlength*opwindfunc(rclosest/r0);
	//if (sop>0.&&sop<0.1) sop=sop*(1-RND*pow((1-sop),3.));
	glColor4d(red,green,blue,sop*lfac*(1.-RND*oprnd));
      }
      glNormal3d(x, y, z);
      glVertex3d(x*radius, y*radius, z*radius);      
      x=sphere->x[index+1];
      y=sphere->y[index+1];
      z=sphere->z[index+1];
      cosalpha=x*dx+y*dy+z*dz;
      lfac=slimba+slimbb*sqrt(cosalpha*cosalpha);
      if (op==0.) glColor3d(lfac*red,lfac*green,lfac*blue);
      else {
	sinalpha=sqrt(1.-cosalpha*cosalpha);
	pathlength=2.*cosalpha;
	rclosest=sinalpha;
	if (rclosest<rmin) {
	  pathlength=pathlength-2.*sqrt(rmin*rmin-rclosest*rclosest);
	  rclosest=rmin;
	}
	sop=0.5*op*pathlength*opwindfunc(rclosest/r0);
	//if (sop>0.&&sop<0.1) sop=sop*(1-RND*pow((1-sop),3.));
	glColor4d(red,green,blue,sop*lfac*(1.-RND*oprnd));
      }
      glNormal3d(x, y, z);
      glVertex3d(x*radius, y*radius, z*radius);
    }
    glEnd();
  }
  
}
