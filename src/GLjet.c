#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <binary.h>
#include "GLjet.h"

void init_jet(jetstruc *jet,double xcentre,double ycentre,double zcentre)
{

  jet->x=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->y=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->z=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->xnormal=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->ynormal=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->znormal=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->red=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->green=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->blue=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->op=(double *)malloc(jet->nz*jet->nphi*sizeof(double));
  jet->xcentre=xcentre;
  jet->ycentre=ycentre;
  jet->zcentre=zcentre;
  calc_jet(jet);
  if (2*(jet->nz/2)==jet->nz)
    {
      jet->nz=jet->nz+1;
      printf("WARNING : nzjet should be odd integer. Using %d.\n",jet->nz);
    }
}

void free_jet(jetstruc *jet)
{
  free(jet->x);
  free(jet->y);
  free(jet->z);
  free(jet->xnormal);
  free(jet->ynormal);
  free(jet->znormal);
  free(jet->red);
  free(jet->green);
  free(jet->blue);
  free(jet->op);
}

/* Calculate secondary star Roche jet surface */
void calc_jet(jetstruc *jet)
{

  double dz,phi,dphi,rjet,drjetdz;
  int iz,iphi,index;
  double x,y,z,nx,ny,nz,nmag,peakradius;

  jet->zpeak=0.5*jet->length*0.25*(3.*(1-jet->rf)+sqrt((1-jet->rf)*(1-jet->rf)+8.*jet->rf*jet->rf));
  peakradius=jet->radius*jet_radius(2.*jet->zpeak/jet->length,jet->rf);

  dz=jet->length/(jet->nz-1);
  dphi=2.*PI/jet->nphi;

  for (iz=0;iz<jet->nz;iz++) {
 
    z=-0.5*jet->length+iz*dz;
    rjet=jet->radius*jet_radius(2.*z/jet->length,jet->rf);

    for (iphi=0;iphi<jet->nphi;iphi++) {
      phi=iphi*dphi;
      index=iz*jet->nphi+iphi;
 
      x=rjet*cos(phi);
      y=rjet*sin(phi);
      jet->x[index]=x;
      jet->y[index]=y;
      jet->z[index]=z;
      
      if (iz==0) {
      	nx=0.;
      	ny=0.;
      	nz=-1.;
      }
      else if (iz==jet->nz-1) {
      	nx=0.;
      	ny=0.;
      	nz=1.;
      }
      else {
	nx=rjet*cos(phi);
	ny=rjet*sin(phi);
	//if (fabs(z)>=jet->zpeak) 
	nz=-rjet*jet->radius*djet_radius_dzf(2.*z/jet->length,jet->rf)*2./jet->length;
	//else nz=0.;
	nmag=sqrt(nx*nx+ny*ny+nz*nz);
	nx=nx/nmag;
	ny=ny/nmag;
	nz=nz/nmag;
      }
      jet->xnormal[index]=nx;
      jet->ynormal[index]=ny;
      jet->znormal[index]=nz;

      jet->red[index]=jet->red1+fabs(2.*z/jet->length)*(jet->red2-jet->red1);
      jet->green[index]=jet->green1+fabs(2.*z/jet->length)*(jet->green2-jet->green1);
      jet->blue[index]=jet->blue1+fabs(2.*z/jet->length)*(jet->blue2-jet->blue1);
      jet->op[index]=(jet->op1+fabs(2.*z/jet->length)*(jet->op2-jet->op1));
      //if (fabs(z)<jet->zpeak) jet->op[index]=jet->op[index]*rjet/peakradius;
    }
  }
}

void draw_jet(jetstruc *jet, char side, double dx,double dy,double dz, systemparsstruc *sys)
{

  double x,y,z,nx,ny,nz,cosalpha,lfac,pathfac;
  int iz,iz1,iz2,iphi,index1,index2;

  if (sys->incdeg>0.) pathfac=1./cos(PI*(90.-sys->incdeg)/180.);
  else pathfac=pathfac=1./cos(PI*(90.-0.01)/180.);

  switch(side)
    {
    case 't':
      iz1=(jet->nz-1)/2;
      iz2=jet->nz-1;
      break;
    case 'b':
      iz1=0;
      iz2=(jet->nz-1)/2-1;
      break;
    default:
      printf("Invalid side to draw_jet()\n");
      return;
      break;
    }

  if (jet->length!=0.) {

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glPushMatrix();
    glTranslated(jet->xcentre,jet->ycentre,jet->zcentre);
    
    for (iz=iz1;iz<iz2;iz++) {
      
      glBegin(GL_QUAD_STRIP);
      for (iphi=0;iphi<=jet->nphi;iphi++) {
	
	index1=iz*jet->nphi+(iphi%jet->nphi);
	x=jet->x[index1];
	y=jet->y[index1];
	z=jet->z[index1];
	nx=jet->xnormal[index1];
	ny=jet->ynormal[index1];
	nz=jet->znormal[index1];
	cosalpha=nx*dx+ny*dy+nz*dz; /* cosine of angle between line of sight and normals */
	if (cosalpha<0.) cosalpha=0.;
	lfac=jet->limba+jet->limbb*cosalpha;
	glColor4d(jet->red[index1],
		  jet->green[index1],
		  jet->blue[index1],
		  jet->op[index1]*lfac*pathfac);
	glNormal3d(nz,ny,nz);
	glVertex3d(x,y,z);      

	index2=index1+jet->nphi;
	x=jet->x[index2];
	y=jet->y[index2];
	z=jet->z[index2];
	nx=jet->xnormal[index2];
	ny=jet->ynormal[index2];
	nz=jet->znormal[index2];
	cosalpha=nx*dx+ny*dy+nz*dz; /* cosine of angle between line of sight and normals */
	if (cosalpha<0.) cosalpha=0.;
	lfac=jet->limba+jet->limbb*cosalpha;
	glColor4d(jet->red[index2],
		  jet->green[index2],
		  jet->blue[index2],
		  jet->op[index2]*lfac*pathfac);
	glNormal3d(nz,ny,nz);
	glVertex3d(x,y,z);      
	
      }
      glEnd();
    }
    
  glPopMatrix();    
  glDisable(GL_CULL_FACE);

  }

}

double jet_radius(double zfrac, double rf)
{

  double r,zf;

  zf=fabs(zfrac);

  r=zf;

  if ((zf>1.-rf)&&(zf<1.)) {
    r=r*sqrt(1.-(zf-1.+rf)*(zf-1.+rf)/rf/rf);
  }
  if (zf>=1.) r=0.;
  
  return(r);

}

double djet_radius_dzf(double zfrac, double rf)
{

  double drdzf,zf,u;

  zf=fabs(zfrac);
  
  drdzf=1.;

  if ((zf>1.-rf)&&(zf<1.)) {
    u=(zf-1.+rf)/rf;
    drdzf=sqrt(1.-u*u)-zf/rf*u/sqrt(1-u*u);
  }
  if (zf>=1.) drdzf=0.; // Actually infinity, but this is dealt with in calc_jet

  if (zfrac<0.) drdzf=-drdzf;

  return(drdzf);

}
