void init_stream(void)
{
  stream.ndraw=stream.n;
  stream.ntheta=20;

  stream.xc=(double *)malloc(stream.n*stream.ntheta*sizeof(double));
  stream.yc=(double *)malloc(stream.n*stream.ntheta*sizeof(double));
  stream.zc=(double *)malloc(stream.n*stream.ntheta*sizeof(double));
  stream.x=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.y=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.z=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.xnormal=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.ynormal=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.znormal=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.red=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.green=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.blue=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));
  stream.op=(double *)malloc(2*stream.n*stream.ntheta*sizeof(double));

  calc_stream();
}

void free_stream(void)
{
  free(stream.xc);
  free(stream.yc);
  free(stream.zc);
  free(stream.x);
  free(stream.y);
  free(stream.z);
  free(stream.xnormal);
  free(stream.ynormal);
  free(stream.znormal);
  free(stream.red);
  free(stream.green);
  free(stream.blue);
  free(stream.op);
}


/* Calculate trajectory of accretion stream */
void calc_stream(void)
{
  double *sx,*sy,*sz,*svix,*sviy,*sviz,*svkx,*svky,*svkz,*sd,*slmag;
  double n1[3],n2[3],s12[3],i1[3],j1[3],i2[3],j2[3],p1[3],p2[3],omega[3],vrel1[3],vrel2[3];
  int d,iang,i,itheta;
  double dtheta,theta,xx1,yy1,xx2,yy2,rstream1,rstream2;
  double fracdist1,fracdist2;
  double norm1[3],norm2[3];

  omega[0]=0.;
  omega[1]=0.;
  omega[2]=2.*PI/sys.Porb;

  svix=(double *)malloc(stream.n*sizeof(double));
  sviy=(double *)malloc(stream.n*sizeof(double));
  sviz=(double *)malloc(stream.n*sizeof(double));
  svkx=(double *)malloc(stream.n*sizeof(double));
  svky=(double *)malloc(stream.n*sizeof(double));
  svkz=(double *)malloc(stream.n*sizeof(double));
  sd=(double *)malloc(stream.n*sizeof(double));
  slmag=(double *)malloc(stream.n*sizeof(double));

  stream.xc[1]=(sys.xl1-0.001)*sys.binary_a;
  stream.yc[1]=0.;
  stream.zc[1]=0.;
  svix[1]=0.;
  sviy[1]=0.;
  sviz[1]=0.;

  stream_calc(sys.q,sys.M1,sys.Porb,
	      sys.Pb,sys.k0,-2.,2.,1.,
	      stream.t,stream.n-1,0,
	      &stream.xc[1],&stream.yc[1],&stream.zc[1],
	      &svix[1],&sviy[1],&sviz[1],
	      &svkx[1],&svky[1],&svkz[1],
	      &sd[1],&slmag[1]);

  stream.xc[0]=(sys.xl1+0.05*(sys.x2-sys.xl1))*sys.binary_a;
  //  stream.xc[0]=x2*binary_a;
  stream.yc[0]=0.;
  stream.zc[0]=0;
  svix[0]=1.;
  sviy[0]=0.;
  sviz[0]=0;
  svkx[0]=1.;
  svky[0]=0.;
  svkz[0]=0;
  sd[0]=0.;
  slmag[0]=slmag[1];

  dtheta=2*PI/stream.ntheta;
  for (i=0;i<stream.n-1;i++)
    {
      p1[0]=stream.xc[i];
      p1[1]=stream.yc[i];
      p1[2]=stream.zc[i];
      p2[0]=stream.xc[i+1];
      p2[1]=stream.yc[i+1];
      p2[2]=stream.zc[i+1];
      s12[0]=p2[0]-p1[0];
      s12[1]=p2[1]-p1[1];
      s12[2]=p2[2]-p1[2];
      cross_vector(vrel1,omega,p1);
      n1[0]=svix[i]-vrel1[0];
      n1[1]=sviy[i]-vrel1[1];
      n1[2]=sviz[i]-vrel1[2];
      cross_vector(vrel2,omega,p2);
      n2[0]=svix[i+1]-vrel2[0];
      n2[1]=sviy[i+1]-vrel2[1];
      n2[2]=sviz[i+1]-vrel2[2];
      cross_vector(i1,n1,s12);
      cross_vector(j1,n1,i1);
      cross_vector(i2,s12,i1); cross_vector(i2,i2,n2);
      cross_vector(j2,n2,i2);
      unit_vector(i1,i1);
      unit_vector(j1,j1);
      unit_vector(i2,i2);
      unit_vector(j2,j2);
      fracdist1=sd[i]/sd[stream.n-1];
      fracdist2=sd[i+1]/sd[stream.n-1];

      for (itheta=0;itheta<stream.ntheta;itheta++)
	{
	  theta=itheta*dtheta;
	  rstream1=(stream.r1+exp(-10.*fracdist1)*(stream.r2-stream.r1))*sys.binary_a;
	  rstream2=(stream.r1+exp(-10.*fracdist2)*(stream.r2-stream.r1))*sys.binary_a;
	  xx1=rstream1*cos(theta);
	  yy1=rstream1*sin(theta);
	  xx2=rstream2*cos(theta);
	  yy2=rstream2*sin(theta);
	  norm1[0]=xx1*i1[0]+yy1*j1[0];
	  norm1[1]=xx1*i1[1]+yy1*j1[1];
	  norm1[2]=xx1*i1[2]+yy1*j1[2];
	  stream.x[itheta+stream.ntheta*i*2]=(p1[0]+norm1[0])/sys.binary_a;
	  stream.y[itheta+stream.ntheta*i*2]=(p1[1]+norm1[1])/sys.binary_a;
	  stream.z[itheta+stream.ntheta*i*2]=(p1[2]+norm1[2])/sys.binary_a;
	  unit_vector(norm1,norm1);
	  stream.xnormal[itheta+stream.ntheta*i*2]=norm1[0];
	  stream.ynormal[itheta+stream.ntheta*i*2]=norm1[1];
	  stream.znormal[itheta+stream.ntheta*i*2]=norm1[2];
	  norm2[0]=xx2*i2[0]+yy2*j2[0];
	  norm2[1]=xx2*i2[1]+yy2*j2[1];
	  norm2[2]=xx2*i2[2]+yy2*j2[2];
	  stream.x[itheta+stream.ntheta+stream.ntheta*i*2]=(p2[0]+norm2[0])/sys.binary_a;
	  stream.y[itheta+stream.ntheta+stream.ntheta*i*2]=(p2[1]+norm2[1])/sys.binary_a;
	  stream.z[itheta+stream.ntheta+stream.ntheta*i*2]=(p2[2]+norm2[2])/sys.binary_a;
	  unit_vector(norm2,norm2);
	  stream.xnormal[itheta+stream.ntheta+stream.ntheta*i*2]=norm2[0];
	  stream.ynormal[itheta+stream.ntheta+stream.ntheta*i*2]=norm2[1];
	  stream.znormal[itheta+stream.ntheta+stream.ntheta*i*2]=norm2[2];
	  stream.red[itheta+stream.ntheta*i*2]=stream.red1+fracdist1*(stream.red2-stream.red1);
	  stream.green[itheta+stream.ntheta*i*2]=stream.green1+fracdist1*(stream.green2-stream.green1);
	  stream.blue[itheta+stream.ntheta*i*2]=stream.blue1+fracdist1*(stream.blue2-stream.blue1);
	  stream.op[itheta+stream.ntheta*i*2]=0.1+(0.7+0.2*RND)*sqrt(fracdist1);
	  stream.red[itheta+stream.ntheta+stream.ntheta*i*2]=stream.red1+fracdist2*(stream.red2-stream.red1);
	  stream.green[itheta+stream.ntheta+stream.ntheta*i*2]=stream.green1+fracdist2*(stream.green2-stream.green1);
	  stream.blue[itheta+stream.ntheta+stream.ntheta*i*2]=stream.blue1+fracdist2*(stream.blue2-stream.blue1);
	  stream.op[itheta+stream.ntheta+stream.ntheta*i*2]=0.1+(0.7+0.2*RND)*sqrt(fracdist2);
	}
    }

  free(svix);
  free(sviy);
  free(sviz);
  free(svkx);
  free(svky);
  free(svkz);
  free(sd);

}

void draw_stream(double dx,double dy,double dz)
{

  int istreamstart,istream,itheta;
  double lfaca,lfacb,costhetaa,costhetab;
  long streamindex1,streamindex2;

  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);
  glDepthMask(GL_FALSE);

  if (ctl.teapot==1) istreamstart=1; else istreamstart=0;
  for (istream=istreamstart;istream<stream.ndraw-1;istream++)
    {
      glBegin(GL_QUAD_STRIP);
      for (itheta=0;itheta<=stream.ntheta;itheta++)
	{
	  streamindex1=itheta%stream.ntheta+stream.ntheta*istream*2;
	  costhetaa=stream.xnormal[streamindex1]*dx+
	    stream.ynormal[streamindex1]*dy+
	    stream.znormal[streamindex1]*dz;
	  lfaca=stream.limba+stream.limbb*sqrt(costhetaa*costhetaa); /* limb darkening factor */
	  //	  glColor4d(lfaca*stream.red[streamindex1], 
	  //	    lfaca*stream.green[streamindex1], 
	  //	    lfaca*stream.blue[streamindex1], 
	  //	    stream.op[streamindex1]); 
	  glColor4d(stream.red[streamindex1], 
		    stream.green[streamindex1], 
		    stream.blue[streamindex1], 
		    lfaca*stream.op[streamindex1]); 
	  glVertex3d(stream.x[streamindex1],
		     stream.y[streamindex1],
		     stream.z[streamindex1]);

	  streamindex2=itheta%stream.ntheta+stream.ntheta+stream.ntheta*istream*2;
	  costhetab=stream.xnormal[streamindex2]*dx+
	    stream.ynormal[streamindex2]*dy+
	    stream.znormal[streamindex2]*dz;
	  lfacb=stream.limba+stream.limbb*sqrt(costhetab*costhetab); /* limb darkening factor */
	  //glColor4d(lfacb*stream.red[streamindex2],
	  //	    lfacb*stream.green[streamindex2],
	  //	    lfacb*stream.blue[streamindex2],
	  //	    stream.op[streamindex2]);
	  glColor4d(stream.red[streamindex2],
		    stream.green[streamindex2],
		    stream.blue[streamindex2],
		    lfacb*stream.op[streamindex2]);
	  glVertex3d(stream.x[streamindex2],
		     stream.y[streamindex2],
		     stream.z[streamindex2]);
	  
	}
      glEnd();
    }
  glDisable(GL_CULL_FACE);
}
