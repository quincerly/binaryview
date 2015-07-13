#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <binary.h>
#include "GLlobe.h"

void init_lobe(lobestruc *lobe, int which, systemparsstruc *sys)
{

  lobe->which=which;
  lobe->r=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  lobe->x=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  lobe->y=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  lobe->z=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  lobe->red=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  lobe->green=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  lobe->blue=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  lobe->irr=(double *)malloc(lobe->ntheta*lobe->nphi*sizeof(double));
  switch(which) {
  case 1:
    lobe->xirr=sys->x2;
    lobe->yirr=0.;
    lobe->zirr=0.;
    lobe->nxirr=0.;
    lobe->nyirr=0.;
    lobe->nzirr=1.;
    break;
  case 2:
    lobe->xirr=sys->x1;
    lobe->yirr=0.;
    lobe->zirr=0.;
    lobe->nxirr=0.;
    lobe->nyirr=0.;
    lobe->nzirr=1.;
    break;
    }

  calc_lobe(lobe,sys);
}

void free_lobe(lobestruc *lobe)
{
  free(lobe->r);
  free(lobe->x);
  free(lobe->y);
  free(lobe->z);
  free(lobe->red);
  free(lobe->green);
  free(lobe->blue);
  free(lobe->irr);
}

/* Calculate secondary star Roche lobe surface */
void calc_lobe(lobestruc *lobe, systemparsstruc *sys)
{

  double theta,dtheta,phi,dphi,ra,rb;
  int itheta,iphi;
  double rsearch,rl,rd,xcentre,xdir;
  double xp,yp,zp,r,rough;
  double theta0=0.,phi0;
  double pot_surf;
  double x,y,z,distirr,dot,intensity;
  double alpha;

  switch(lobe->which) {
  case 1:
    xcentre=sys->x1;
    xdir=-1.;
    rl=sys->rl1;
    rd=sys->rd1;
    rsearch=lobe->rfrac*rl;
    phi0=0.75*PI;
    break;
  case 2:
    xcentre=sys->x2;
    xdir=1.;
    rl=sys->rl2;
    rd=sys->rd2;
    rsearch=lobe->rfrac*rl;
    phi0=0.25*PI;
    break;
  }

  pot_surf=roche_potl(xcentre-xdir*lobe->rfrac*rl,0.,0.,sys->q);

  dtheta=PI/(lobe->ntheta-1);
  dphi=xdir*2*PI/lobe->nphi;

  for (itheta=0;itheta<lobe->ntheta;itheta++) {
    theta=theta0+itheta*dtheta;
    for (iphi=0;iphi<lobe->nphi;iphi++) {
      phi=phi0+iphi*dphi;      
      xp=xcentre-xdir*rsearch*cos(theta);
      yp=-rsearch*cos(phi)*sin(theta);
      zp=rsearch*sin(phi)*sin(theta);
      //      r=find_potl(pot_surf,sys->q,sys->x2,0.,0.,xp,yp,zp)*rsearch;
      r=find_potl(pot_surf,sys->q,xcentre,0.,0.,xp,yp,zp)*rsearch;
      x=-xdir*r*cos(theta);
      y=-r*cos(phi)*sin(theta);
      z=r*sin(phi)*sin(theta);
      lobe->r[itheta+lobe->ntheta*iphi]=r;
      lobe->x[itheta+lobe->ntheta*iphi]=x;
      lobe->y[itheta+lobe->ntheta*iphi]=y;
      lobe->z[itheta+lobe->ntheta*iphi]=z;
      //rough=RND;
      //lobe->red[itheta+lobe->ntheta*iphi]=lobe->red1
      //+(r-rd)/(rl-rd)*(lobe->red2-lobe->red1)-rough*lobe->rough;
      //lobe->green[itheta+lobe->ntheta*iphi]=lobe->green1
      //	+(r-rd)/(rl-rd)*(lobe->green2-lobe->green1)-rough*lobe->rough;
      //lobe->blue[itheta+lobe->ntheta*iphi]=lobe->blue1
      //	+(r-rd)/(rl-rd)*(lobe->blue2-lobe->blue1)-rough*lobe->rough;
      lobe->red[itheta+lobe->ntheta*iphi]=lobe->red1
      	+(r-rd)/(rl-rd)*(lobe->red2-lobe->red1);
      lobe->green[itheta+lobe->ntheta*iphi]=lobe->green1
      	+(r-rd)/(rl-rd)*(lobe->green2-lobe->green1);
      lobe->blue[itheta+lobe->ntheta*iphi]=lobe->blue1
      	+(r-rd)/(rl-rd)*(lobe->blue2-lobe->blue1);
      
      // Calculate irradiation from point xirr,yirr,zirr=Rirr
      // Assume region at R is irradiated proportional to
      // cos(theta)/|Rirr-R|^2 where cos(theta)=angle between
      // normal at R and line from R to Rirr.
      // cos(theta)=Normal.(Rirr-R)/|Normal||Rirr-R| at point R

      // Distance to source
      distirr=sqrt((lobe->xirr-x-xcentre)*(lobe->xirr-x-xcentre)+
		   (lobe->yirr-y)*(lobe->yirr-y)+
		   (lobe->zirr-z)*(lobe->zirr-z));

      // Intensity of source at this point
      dot=x/r*(lobe->xirr-x-xcentre)+y/r*(lobe->yirr-y)+z/r*(lobe->zirr-z);
      intensity=dot/distirr/distirr/distirr;
      if (intensity<0.) {

	// No irradiation at this point
	intensity=0.;

      }
      else {

	// Is disc obscured by flared disc?
	dot=lobe->nxirr*(lobe->xirr-x-xcentre)+
	  lobe->nyirr*(lobe->yirr-y)+
	  lobe->nzirr*(lobe->zirr-z);
	dot=fabs(dot/distirr);
	alpha=90.-acos(dot)/PI*180.;

	// Smoothly reduce irradiation to zero over 0.2 flareirr for alpha<flareirr
	if (alpha<lobe->flareirr)
	  intensity=intensity*
	    exp(-(lobe->flareirr-alpha)*(lobe->flareirr-alpha)/lobe->dflareirr/lobe->dflareirr);
      }
      lobe->irr[itheta+lobe->ntheta*iphi]=intensity;
    }
  }
}

void draw_lobe(lobestruc *lobe, double dx,double dy,double dz, systemparsstruc *sys, controlstruc *ctl)
{

  double teapotsize;
  double ra,rb;
  double xa,ya,za,xb,yb,zb;
  double xcentre;
  double rough;

  int itheta,iphi,nthetacut,nphicut,ircut,nrcut;
  int phimin,phimax;
  double lfaca,lfacb,costhetaa,costhetab,irr;
  double rgbcut[3];
  double xcut1,ycut1,zcut1,xcut2,ycut2,zcut2,rfraccut;
  long lobeindex,lobeindex1,lobeindex2;

  float ambient[] = {0.9, 0.9, 0.9};
  float diffuse[] = {1., 1., 1.};
  float specular[] = {1., 1., 1.};
  float position[] = {sys->x1,.0,.0, 0.0};
  float local_view[] = {0.0};

  switch(lobe->which) {
  case 1:
    xcentre=sys->x1;
    break;
  case 2:
    xcentre=sys->x2;
    break;
  }

  nrcut=20;
  nthetacut=(int)(0.5*lobe->ntheta);
  nphicut=(int)(0.25*lobe->nphi);

  if ((ctl->teapot==1)&&(lobe->which==2)) {
    /* Draw donor teapot */
    glDisable(GL_BLEND); 
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    teapotsize=0.24*(sys->x2-sys->xl1)/0.396;
    glPushMatrix();
    glTranslated(sys->x2,0.,0.);
    glRotated(180.,0.,0.,1.);
    glRotated(90.,1.,0.,0.);
    glRotated(-16.,0.,0.,1.);
    glColor3d(lobe->red1,lobe->green1,lobe->blue1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.6*128.0);
    glutSolidTeapot(teapotsize);
    glPopMatrix();    
    glDisable(GL_LIGHTING);
  glEnable(GL_BLEND); 
  }
  else
    {    
      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
      glDepthMask(GL_TRUE);
      glPushMatrix();
      glTranslated(xcentre,0.,0.);
      /* Draw secondary star */
      for (itheta=0;itheta<lobe->ntheta-1;itheta++) {

	// Leave out cutaway surface if required
	if ((itheta<nthetacut)&&(lobe->cutaway==1))
	  {
	    phimin=(int)(nphicut);
	    phimax=lobe->nphi;
	  }
	else
	  {
	    phimin=0;
	    phimax=lobe->nphi;
	  }

	glBegin(GL_QUAD_STRIP);
	for (iphi=phimin;iphi<=phimax;iphi++) {
	  lobeindex=itheta+lobe->ntheta*(iphi%lobe->nphi);
	  ra=lobe->r[lobeindex];
	  xa=lobe->x[lobeindex];
	  ya=lobe->y[lobeindex];
	  za=lobe->z[lobeindex];
	  costhetaa=xa/ra*dx+ya/ra*dy+za/ra*dz; /* cosine of angle between line of sight and normals */
	  if (costhetaa<0) costhetaa=0.;
	  lfaca=lobe->limba+lobe->limbb*costhetaa;      /* limb darkening factor */
	  irr=lobe->irr[lobeindex]*lobe->intirr;
	  rough=RND;
	  glColor3d(lfaca*lobe->red[lobeindex]+irr*lobe->redirr-rough*lobe->rough,
		    lfaca*lobe->green[lobeindex]+irr*lobe->greenirr-rough*lobe->rough,
		    lfaca*lobe->blue[lobeindex]+irr*lobe->blueirr-rough*lobe->rough);
	  glNormal3d(xa/ra, ya/ra, za/ra);
	  glVertex3d(xa, ya, za);      
	  rb=lobe->r[lobeindex+1];
	  xb=lobe->x[lobeindex+1];
	  yb=lobe->y[lobeindex+1];
	  zb=lobe->z[lobeindex+1];
	  costhetab=xb/rb*dx+yb/rb*dy+zb/rb*dz;
	  if (costhetab<0) costhetab=0.;
	  lfacb=lobe->limba+lobe->limbb*costhetab;
	  irr=lobe->irr[lobeindex+1]*lobe->intirr;
	  rough=RND;
	  glColor3d(lfacb*lobe->red[lobeindex+1]+irr*lobe->redirr-rough*lobe->rough,
		    lfacb*lobe->green[lobeindex+1]+irr*lobe->greenirr-rough*lobe->rough,
		    lfacb*lobe->blue[lobeindex+1]+irr*lobe->blueirr-rough*lobe->rough);
	  glNormal3d(xb/rb, yb/rb, zb/rb);
	  glVertex3d(xb, yb, zb);
	}
	glEnd();
      }

      if (lobe->cutaway==1) {

	// Draw cutway section 1
	itheta=nthetacut;
	for (iphi=0;iphi<nphicut;iphi++) {
	  lobeindex1=itheta+lobe->ntheta*(iphi%lobe->nphi);
	  lobeindex2=itheta+lobe->ntheta*((iphi+1)%lobe->nphi);
	  xcut1=lobe->x[lobeindex1];
	  ycut1=lobe->y[lobeindex1];
	  zcut1=lobe->z[lobeindex1];
	  xcut2=lobe->x[lobeindex2];
	  ycut2=lobe->y[lobeindex2];
	  zcut2=lobe->z[lobeindex2];
	  glBegin(GL_QUAD_STRIP);
	  for (ircut=0;ircut<=nrcut;ircut++) {
	    rfraccut=(double)ircut/nrcut;
	    cutawaycolour(rgbcut,1,rfraccut,0.);
	    glColor3d(rgbcut[0],rgbcut[1],rgbcut[2]);
	    glNormal3d(-1.,0.,0.);
	    glVertex3d(xcut2*rfraccut, ycut2*rfraccut, zcut2*rfraccut);
	    cutawaycolour(rgbcut,1,rfraccut,0.);
	    glColor3d(rgbcut[0],rgbcut[1],rgbcut[2]);
	    glNormal3d(-1.,0.,0.);
	    glVertex3d(xcut1*rfraccut, ycut1*rfraccut, zcut1*rfraccut);
	  }
	  glEnd();      	
	}

	// Draw cutway section 2
	iphi=0;
	for (itheta=0;itheta<nthetacut;itheta++) {
	  lobeindex1=itheta+lobe->ntheta*(iphi%lobe->nphi);
	  lobeindex2=itheta+1+lobe->ntheta*(iphi%lobe->nphi);
	  xcut1=lobe->x[lobeindex1];
	  ycut1=lobe->y[lobeindex1];
	  zcut1=lobe->z[lobeindex1];
	  xcut2=lobe->x[lobeindex2];
	  ycut2=lobe->y[lobeindex2];
	  zcut2=lobe->z[lobeindex2];
	  glBegin(GL_QUAD_STRIP);
	  for (ircut=0;ircut<=nrcut;ircut++) {
	    rfraccut=(double)ircut/nrcut;
	    cutawaycolour(rgbcut,2,rfraccut,0.);
	    glColor3d(rgbcut[0],rgbcut[1],rgbcut[2]);
	    glNormal3d(-1.,0.,0.);
	    glVertex3d(xcut2*rfraccut, ycut2*rfraccut, zcut2*rfraccut);
	    cutawaycolour(rgbcut,2,rfraccut,0.);
	    glColor3d(rgbcut[0],rgbcut[1],rgbcut[2]);
	    glNormal3d(-1.,0.,0.);
	    glVertex3d(xcut1*rfraccut, ycut1*rfraccut, zcut1*rfraccut);
	  }
	  glEnd();      	
	}

	// Draw cutway section 3
	iphi=nphicut;
	for (itheta=0;itheta<nthetacut;itheta++) {
	  lobeindex1=itheta+lobe->ntheta*(iphi%lobe->nphi);
	  lobeindex2=itheta+1+lobe->ntheta*(iphi%lobe->nphi);
	  xcut1=lobe->x[lobeindex1];
	  ycut1=lobe->y[lobeindex1];
	  zcut1=lobe->z[lobeindex1];
	  xcut2=lobe->x[lobeindex2];
	  ycut2=lobe->y[lobeindex2];
	  zcut2=lobe->z[lobeindex2];
	  glBegin(GL_QUAD_STRIP);
	  for (ircut=0;ircut<=nrcut;ircut++) {
	    rfraccut=(double)ircut/nrcut;
	    cutawaycolour(rgbcut,3,rfraccut,0.);
	    glColor3d(rgbcut[0],rgbcut[1],rgbcut[2]);
	    glNormal3d(-1.,0.,0.);
	    glVertex3d(xcut1*rfraccut, ycut1*rfraccut, zcut1*rfraccut);
	    cutawaycolour(rgbcut,3,rfraccut,0.);
	    glColor3d(rgbcut[0],rgbcut[1],rgbcut[2]);
	    glNormal3d(-1.,0.,0.);
	    glVertex3d(xcut2*rfraccut, ycut2*rfraccut, zcut2*rfraccut);

	  }
	  glEnd();      	
	}

      }      
      glPopMatrix();    
    }

}

void cutawaycolour(double *rgb, int face, double r, double time)
{

  float lcore,rcore=0.2,drcoresq=0.0025;
  if (r<=rcore) lcore=1.; else lcore=exp(-(r-rcore)*(r-rcore)/drcoresq);

  switch(face) {
  case 1:
    rgb[0]=lcore+(1.-r)*(0.8+0.2*RND);
    rgb[1]=lcore+(1.-r)*0.5;
    rgb[2]=lcore+(1.-r)*0.5;
    break;
  case 2:
    rgb[0]=lcore+(1.-r)*0.5;
    rgb[1]=lcore+(1.-r)*(0.8+0.2*RND);
    rgb[2]=lcore+(1.-r)*0.5;
    break;
  case 3:
    rgb[0]=lcore+(1.-r)*0.5;
    rgb[1]=lcore+(1.-r)*0.5;
    rgb[2]=lcore+(1.-r)*(0.8+0.2*RND);
    break;
  default:
    rgb[0]=1.;
    rgb[1]=1.;
    rgb[2]=1.;
    break;
  }
}
