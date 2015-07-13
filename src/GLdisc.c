void init_disc(void)
{
  disc.x0=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.y0=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.z0=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.red_flare=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.green_flare=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.blue_flare=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.red_spot=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.green_spot=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.blue_spot=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.x=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.y=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.z=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.red=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.green=(double *)malloc(disc.nr*disc.nbeta*sizeof(double));
  disc.blue=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.op=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.op_flare=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.op_spot=(double *)malloc(2*disc.nr*disc.nbeta*sizeof(double));
  disc.kepperiod=(double *)malloc(disc.nr*sizeof(double));

  calc_disc();
}

void free_disc(void)
{
  free(disc.x0);
  free(disc.y0);
  free(disc.z0);
  free(disc.x);
  free(disc.y);
  free(disc.z);
  free(disc.red);
  free(disc.green);
  free(disc.blue);
  free(disc.red_spot);
  free(disc.green_spot);
  free(disc.blue_spot);
  free(disc.red_flare);
  free(disc.green_flare);
  free(disc.blue_flare);
  free(disc.op);
  free(disc.op_spot);
  free(disc.op_flare);
  free(disc.kepperiod);
}

/* Calculate disc outer radius and angle beta */
double calc_rdisc2(double beta)
{
  double A,B,C;
  A=sin(beta)*sin(beta)+sqrt(1.-disc.e*disc.e)*cos(beta)*cos(beta);
  B=-2.*disc.a*disc.e*sqrt(1.-disc.e*disc.e)*cos(beta);
  C=disc.a*disc.a*sqrt(1.-disc.e*disc.e)*(disc.e*disc.e-1.);
  return((-B+sqrt(B*B-4.*A*C))/2./A);
}

/* Calculate disc `top' and `bottom' surfaces */
void calc_disc(void)
{

  double r,beta,dbeta,za,zb,rdisc1,rdisc2,rfrac,rfracmin,discheight,rcutoff;
  int ir,ibeta;
  double flarebeta,flaredbeta=PI*0.2,flarer,flaredr=0.05,flarebrightness;
  double rdist,betadist,intensity,hotspotr,hotspotbeta;
  int nrflare,flare,nflares=50;

  dbeta=2*PI/disc.nbeta;
  for (ir=0;ir<disc.nr;ir++) {
    for (ibeta=0;ibeta<disc.nbeta;ibeta++) {
      beta=ibeta*dbeta;
      rdisc1=disc.radius1;
      rdisc2=calc_rdisc2(beta);
      r=rdisc1+ir*(rdisc2-rdisc1)/(disc.nr-1);
      rfracmin=rdisc1/(disc.a*(1-disc.e)-rdisc1);
      rfrac=(r-rdisc1)/(rdisc2-rdisc1)+rfracmin;
      discheight=disc.thickness*r*r/rdisc2/rdisc2;
      rcutoff=0.9*rdisc2;
      if ((r>rcutoff)&&(r!=rdisc2)) discheight=discheight*sqrt(1.-(r-rcutoff)*(r-rcutoff)/(rdisc2-rcutoff)/(rdisc2-rcutoff));
      if ((r>rcutoff)&&(r==rdisc2)) discheight=0.;
      disc.x[ir+disc.nr*ibeta]=r*cos(beta);
      disc.y[ir+disc.nr*ibeta]=r*sin(beta);
      disc.z[ir+disc.nr*ibeta]=discheight;
      disc.z[ir+disc.nr*ibeta+disc.nr*disc.nbeta]=-discheight;
      disc.red[ir+disc.nr*ibeta]=disc.red1-(disc.red1-disc.red2)*(rfrac-rfracmin);
      disc.green[ir+disc.nr*ibeta]=disc.green1-(disc.green1-disc.green2)*(rfrac-rfracmin);
      disc.blue[ir+disc.nr*ibeta]=disc.blue1-(disc.blue1-disc.blue2)*(rfrac-rfracmin);
      disc.op[ir+disc.nr*ibeta]=disc.op1-(disc.op1-disc.op2)*(rfrac-rfracmin);
      //      disc.op[ir+disc.nr*ibeta]=(0.9-0.5*pow(rfrac-rfracmin,1.5))*(1.-0.2*RND);
      disc.x0[ir+disc.nr*ibeta]=disc.x[ir+disc.nr*ibeta];
      disc.y0[ir+disc.nr*ibeta]=disc.y[ir+disc.nr*ibeta];
      disc.z0[ir+disc.nr*ibeta]=disc.z[ir+disc.nr*ibeta];
      disc.z0[ir+disc.nr*ibeta+disc.nr*disc.nbeta]=disc.z[ir+disc.nr*ibeta+disc.nr*disc.nbeta];
    }
    disc.kepperiod[ir]=2.*PI/sqrt(G*sys.M1*Msun/pow(r*sys.binary_a,3.));
  }

  // Add pretty flares a la Rob
  for (ir=0;ir<disc.nr;ir++) {
    for (ibeta=0;ibeta<disc.nbeta;ibeta++) {
      disc.red_flare[ir+disc.nr*ibeta]=0.;
      disc.green_flare[ir+disc.nr*ibeta]=0.;
      disc.blue_flare[ir+disc.nr*ibeta]=0.;      
    }
  }
  for (flare=0;flare<nflares;flare++) {
    flarebeta=(int)(RND*(disc.nbeta-1));
    flarer=(int)(RND*(disc.nr-1));
    flarebrightness=RND;
    for (ir=0;ir<disc.nr;ir++) {
      for (ibeta=0;ibeta<disc.nbeta;ibeta++) {
  	rdist=((double)(ir-flarer));
  	betadist=(double)(ibeta-flarebeta);
  	betadist=sqrt(betadist*betadist)/disc.nbeta;
  	if (betadist>0.5) betadist=1.-betadist;
  	intensity=exp(-rdist*rdist-betadist*betadist/0.01);
  	disc.red_flare[ir+disc.nr*ibeta]=disc.red_flare[ir+disc.nr*ibeta]+0.5*intensity*flarebrightness;
  	disc.green_flare[ir+disc.nr*ibeta]=disc.green_flare[ir+disc.nr*ibeta]+0.5*intensity*flarebrightness;
  	disc.blue_flare[ir+disc.nr*ibeta]=disc.blue_flare[ir+disc.nr*ibeta]+0.5*intensity*flarebrightness;
	disc.op_flare[ir+disc.nr*ibeta]=1.;
	disc.op_spot[ir+disc.nr*ibeta]=1.;
     }
    } 
  }
}

/* Calculate disc `top' and `bottom' surfaces */
void calc_hotspot(void)
{
  
  double r,beta,dbeta,za,zb,rdisc1,rdisc2,rfrac,rfracmin,alpha;
  int ir,ibeta;
  double hotspotdr=spot.dradius*disc.nr,hotspotheight=2.;
  double hotspotdbeta1=spot.dphase1*disc.nbeta,hotspotdbeta2=spot.dphase2*disc.nbeta;
  double rdist,betadist,betadist0,intensity,hotspotr,hotspotbeta;
  double hotspotangle;
  
  if (disc.a>0.) {

    // Add hot spot
    hotspotangle=spot.phase+sys.shphase;
    hotspotangle=hotspotangle-(int)hotspotangle;
    if (hotspotangle>0.5) hotspotangle=hotspotangle-1.;
    hotspotbeta=hotspotangle*(double)(disc.nbeta-1);
    hotspotr=spot.radius*(double)(disc.nr-1);
    
    for (ir=0;ir<disc.nr;ir++) {
      for (ibeta=0;ibeta<disc.nbeta;ibeta++) {
	rdist=(double)ir-hotspotr;
	betadist=(double)ibeta-hotspotbeta;
	if (betadist>0.5*(double)disc.nbeta) betadist=-1.*((double)disc.nbeta-betadist);
	if (betadist<0.) intensity=exp(-rdist*rdist/hotspotdr/hotspotdr-betadist*betadist/hotspotdbeta1/hotspotdbeta1);
	else intensity=exp(-rdist*rdist/hotspotdr/hotspotdr-betadist*betadist/hotspotdbeta2/hotspotdbeta2);
	disc.z[ir+disc.nr*ibeta]=disc.z0[ir+disc.nr*ibeta]*(1.+hotspotheight*intensity);
	disc.z[ir+disc.nr*ibeta+disc.nr*disc.nbeta]=disc.z0[ir+disc.nr*ibeta+disc.nr*disc.nbeta]*(1.+hotspotheight*intensity);
	disc.red_spot[ir+disc.nr*ibeta]=spot.red*intensity;
	disc.green_spot[ir+disc.nr*ibeta]=spot.green*intensity;
	disc.blue_spot[ir+disc.nr*ibeta]=spot.blue*intensity;
	disc.op_spot[ir+disc.nr*ibeta]=1.;
      }
    }  
  }
}

void calc_spotpos(void)
{

  double r_stream;

  /* Calculate spot position */
  if (disc.a<=0.) stream.ndraw=stream.n;
  else {
    stream.ndraw=0;
    do {
      r_stream=sqrt((stream.xc[stream.ndraw]/sys.binary_a-sys.x1)*(stream.xc[stream.ndraw]/sys.binary_a-sys.x1)+stream.yc[stream.ndraw]/sys.binary_a*stream.yc[stream.ndraw]/sys.binary_a);
      spot.theta_pos=angle(stream.xc[stream.ndraw]/sys.binary_a-sys.x1,stream.yc[stream.ndraw]/sys.binary_a);
      stream.ndraw++;
    } while (calc_rdisc2(spot.theta_pos+sys.shphase*2.*PI)*spot.radius<r_stream);
    spot.rad_pos=calc_rdisc2(spot.theta_pos+sys.shphase*2.*PI)*spot.radius;
    spot.phase=spot.theta_pos/2./PI;
    printf("%g %g\n",spot.rad_pos,spot.phase);
  }
}

void draw_disc(void)
{

  double beta,dbeta,dr;
  int ir,ibeta,discside;
  long discindex,discindexkep;
  int iphi1,iphi2;
  int betaoffset;
  double inc;

  glDepthMask(GL_FALSE);
  if (disc.a!=0.) {

    if (sys.incdeg!=90.) inc=sys.incdeg/360.*PI; else inc=89.999/360.*PI;
    dbeta=2*PI/disc.nbeta;
    glPushMatrix();
    glRotated(-sys.shphase*360.,0.,0.,1.);
    glTranslated(sys.x1*cos(-sys.shphase*2*PI),-sys.x1*sin(-sys.shphase*2*PI),0.);  
    
    for (discside=0;discside<2;discside++) {
      for (ir=0;ir<disc.nr-1;ir++) {

	// Offset in beta for Keplerian rotation
	betaoffset=disc.nbeta-(int)(disc.nbeta*fold(ctl.realtime/disc.kepperiod[ir]));

	glBegin(GL_QUAD_STRIP);
	for (ibeta=0;ibeta<=disc.nbeta;ibeta++) {
	  
	  beta=ibeta*dbeta;
	  discindex=ir+disc.nr*(ibeta%disc.nbeta);
	  discindexkep=ir+disc.nr*((ibeta+betaoffset)%disc.nbeta);
	  
	  glColor4d(disc.red[discindexkep]+disc.red_flare[discindexkep]+disc.red_spot[discindex]-0.1*RND,
		    disc.green[discindexkep]+disc.green_flare[discindexkep]+disc.green_spot[discindex],
		    disc.blue[discindexkep]+disc.blue_flare[discindexkep]+disc.blue_spot[discindex]-0.1*RND,
		    //1.);
		    disc.op[discindexkep]*disc.op_flare[discindexkep]*disc.op_spot[discindex]/cos(inc));
	  glVertex3d(disc.x[discindex],disc.y[discindex],disc.z[discindex+discside*disc.nr*disc.nbeta]);
	  
	  glColor4d(disc.red[discindexkep+1]+disc.red_flare[discindexkep+1]+disc.red_spot[discindex+1]-0.1*RND,
		    disc.green[discindexkep+1]+disc.green_flare[discindexkep+1]+disc.green_spot[discindex+1],
		    disc.blue[discindexkep+1]+disc.blue_flare[discindexkep+1]+disc.blue_spot[discindex+1]-0.1*RND,
		    //1.);
		    disc.op[discindexkep+1]*disc.op_flare[discindexkep+1]*disc.op_spot[discindex+1]/cos(inc));
	  glVertex3d(disc.x[discindex+1],disc.y[discindex+1],disc.z[discindex+1+discside*disc.nr*disc.nbeta]);
	}
	
	glEnd();
      }
    }
    glPopMatrix();
  }    
}
