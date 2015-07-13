/* Return unit vector of b as a */
void unit_vector(double *a,double *b)
{
  double magnitude=sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);

  a[0]=b[0]/magnitude;
  a[1]=b[1]/magnitude;
  a[2]=b[2]/magnitude;

}

void init_all(void)
{
  srand(time(NULL)); /* Seed random numbers */

  ctl.centre[0]=0.;
  ctl.centre[1]=0.;
  ctl.centre[2]=0.;
  ctl.objectComplexity=2000000;
  ctl.transparent=1;

  init_sys();
  init_sphere();
  init_lobe(&donor,2,&sys);
  init_wind(&donorwind,sys.x2,0.,0.);
  init_lobe(&accretor,1,&sys);
  init_wind(&accretorwind,sys.x1,0.,0.);
  init_disc();
  init_jet(&accretorjet,sys.x1,0.,0.);
  if (ctl.streamflag>0) init_stream();
  init_starfield();
}
void free_all(void)
{
  free_sphere();
  free_lobe(&donor);
  free_lobe(&accretor);
  free_disc();
  free_jet(&accretorjet);
  if (ctl.streamflag>0) free_stream();
  free_starfield();
}

void init_sys(void)
{
  sys.phasedeg=sys.orbphase0*360.;
  sys.binary_a=binary_sep(sys.q,sys.M1,sys.Porb);
  sys.x1=-sys.q/(1.+sys.q);
  sys.x2=1./(1.+sys.q);
  sys.xl1=roche_xl1(sys.q);
  sys.rl1=sys.xl1-sys.x1;
  sys.rl2=sys.x2-sys.xl1;
  sys.potl1=roche_potl(sys.xl1,0.,0.,sys.q);
  sys.rd1=find_potl(sys.potl1,sys.q,sys.x1,0.,0.,sys.x1,0.,sys.x2-sys.xl1)*(sys.x2-sys.xl1);
  sys.rd2=find_potl(sys.potl1,sys.q,sys.x2,0.,0.,sys.x2,0.,sys.x2-sys.xl1)*(sys.x2-sys.xl1);
}

void init_sphere(void)
{
  sphere.ntheta=50;
  sphere.nphi=50;

  sphere.x=(double *)malloc(sphere.ntheta*sphere.nphi*sizeof(double));
  sphere.y=(double *)malloc(sphere.ntheta*sphere.nphi*sizeof(double));
  sphere.z=(double *)malloc(sphere.ntheta*sphere.nphi*sizeof(double));

  calc_sphere();
}
void free_sphere(void)
{
  free(sphere.x);
  free(sphere.y);
  free(sphere.z);
}

void init_starfield(void)
{
  starfield.x=(double *)malloc(starfield.n*sizeof(double));
  starfield.y=(double *)malloc(starfield.n*sizeof(double));
  starfield.z=(double *)malloc(starfield.n*sizeof(double));
  starfield.r=(double *)malloc(starfield.n*sizeof(double));
  starfield.red=(double *)malloc(starfield.n*sizeof(double));
  starfield.green=(double *)malloc(starfield.n*sizeof(double));
  starfield.blue=(double *)malloc(starfield.n*sizeof(double));

  calc_starfield();
}
void free_starfield(void)
{
  free(starfield.x);
  free(starfield.y);
  free(starfield.z);
  free(starfield.r);
  free(starfield.red);
  free(starfield.green);
  free(starfield.blue);
}

/* Generate starfield */
void calc_starfield(void)
{
  int star;
  double startheta,starphi,maxcol;

  for (star=0;star<starfield.n;star++) {
    startheta=(RND-0.5)*0.2*PI*0.5+0.5*PI;
    starphi=(RND-0.5)*PI+PI-sys.incdeg/180.*PI;    
    starfield.x[star]=10.*cos(startheta);
    starfield.y[star]=10.*sin(startheta)*sin(starphi);
    starfield.z[star]=10.*sin(startheta)*cos(starphi);
    starfield.r[star]=RND*0.008;
    maxcol=RND;
    starfield.red[star]=maxcol*(1-0.3*RND*RND);
    starfield.blue[star]=maxcol*(1-0.3*RND*RND);
    starfield.green[star]=(starfield.red[star]+starfield.blue[star])*0.5;
  }
}

/* Calculate secondary star Roche lobe surface */
void calc_sphere(void)
{

  double theta,dtheta,phi,dphi;
  int itheta,iphi;

  dtheta=PI/(sphere.ntheta-1);
  dphi=2*PI/sphere.nphi;
  for (itheta=0;itheta<sphere.ntheta;itheta++) {
    theta=itheta*dtheta;
    for (iphi=0;iphi<sphere.nphi;iphi++) {
      phi=iphi*dphi;      
      sphere.x[itheta+sphere.ntheta*iphi]=cos(theta);
      sphere.y[itheta+sphere.ntheta*iphi]=cos(phi)*sin(theta);
      sphere.z[itheta+sphere.ntheta*iphi]=sin(phi)*sin(theta);
    }
  }
}

void draw_sphere(double radius, double red, double green, double blue,
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

  dtheta=PI/(sphere.ntheta-1);
  dphi=2*PI/sphere.nphi;
  for (itheta=0;itheta<sphere.ntheta-1;itheta++) {
    glBegin(GL_QUAD_STRIP);
    for (iphi=0;iphi<=sphere.nphi;iphi++) {
      phi=iphi*dphi;
      index=itheta+sphere.ntheta*(iphi%sphere.nphi);
      x=sphere.x[index];
      y=sphere.y[index];
      z=sphere.z[index];
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
      x=sphere.x[index+1];
      y=sphere.y[index+1];
      z=sphere.z[index+1];
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

void render(void)
{

  int star;
  double dx,dy,dz;
  double hw,wh;

  wh=(double)ctl.window_width/ctl.window_height;
  hw=(double)ctl.window_height/ctl.window_width;

  /* Update spot position and structure */
  if (ctl.streamflag>0) {
      calc_spotpos();
      calc_hotspot();
  }
  
  /* Set correct viewing angle */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-1.3, 1.3, -1.3, 1.3, -10., 10.);
  //glScaled(1.,(double)ctl.window_width/ctl.window_height,1.);
  if (wh<=0.)
    glOrtho(-1.3, 1.3, -1.3*hw, 1.3*hw, -10., 10.);
  else
    glOrtho(-1.3*wh, 1.3*wh, -1.3, 1.3, -10., 10.);
  glRotated(-sys.incdeg,1.,0.,0.);
  glMatrixMode(GL_MODELVIEW);

  /* Draw starfield */
  for (star=0;star<starfield.n;star++) {
    glColor3d(starfield.red[star],starfield.green[star],starfield.blue[star]);    
    glPushMatrix();
    glTranslated(starfield.x[star],starfield.y[star],starfield.z[star]);
    glutSolidSphere(starfield.r[star],6.,6.);
    glPopMatrix();
  }

  /* Draw the system */
  glPushMatrix();
  glScaled(ctl.scale,ctl.scale,ctl.scale);
  glRotated(sys.phasedeg-90.,0.,0.,1.);
  glTranslated(-ctl.centre[0],-ctl.centre[1],-ctl.centre[2]); /* Offset for whole sysem */
  dxdydz(&dx,&dy,&dz,sys.incdeg,sys.phasedeg/360.); /* Direction of observer at this phase */

  /* Set up transparency */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND); 

  /* Donor */
  draw_lobe(&donor,dx,dy,dz,&sys,&ctl);

  /* Accretor */
  draw_lobe(&accretor,dx,dy,dz,&sys,&ctl);

  /* Bottom of jet from accretor */
  draw_jet(&accretorjet,'b',dx,dy,dz,&sys);

  /* Disc around accretor */
  draw_disc();

  /* Top of jet from accretor */
  draw_jet(&accretorjet,'t',dx,dy,dz,&sys);

  /* Accretion stream */
  if (ctl.streamflag>0) draw_stream(dx,dy,dz);

  draw_wind(&donorwind,dx,dy,dz);
  draw_wind(&accretorwind,dx,dy,dz);

  /* Fancy hot spot */
  if ((ctl.streamflag>0)&&(disc.a!=0.)&&(spot.atmos==1.)&&(ctl.transparent==1)) {
    glPushMatrix();
    glTranslated(sys.x1+spot.rad_pos*1.02*cos(spot.theta_pos),spot.rad_pos*1.02*sin(spot.theta_pos),0.);
    glDepthMask(GL_FALSE);
    draw_sphere(spot.dradius*disc.a*3.,spot.red,spot.green,spot.blue,
		0.,1.,
		1.,0.5,
		0.,0.,
		dx,dy,dz);
    glPopMatrix();
  }

  /* Finish transparency stuff */
  glDepthMask(GL_TRUE);

  glPopMatrix();

}
