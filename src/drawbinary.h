#ifndef __DRAWBINARY
#define __DRAWBINARY
#ifdef __cplusplus
extern "C" {
#endif
  
#define RND ((double)rand()/RAND_MAX)
#define PI 3.1415926536
#define G 6.672*1e-11
#define Msun 1.9891*1e30

typedef struct
{

  double q,incdeg,x1,x2,xl1,rl1,rl2,potl1,rd1,rd2,periodsec,phasedeg,binary_a;
  double epsilon;
  double M1,Porb,Pb,k0;
  double orbphase0;
  double shphase;

} systemparsstruc;

typedef struct
{

  double radius1,a,thickness,e,shphase0;
  int nr,nbeta;
  double red1,green1,blue1,red2,green2,blue2;
  double op1,op2;
  double *x,*y,*z;
  double *red,*green,*blue,*op;
  double *red_flare,*green_flare,*blue_flare,*op_flare;
  double *red_spot,*green_spot,*blue_spot,*op_spot;
  double *x0,*y0,*z0;
  double *kepperiod;

} discstruc;

typedef struct
{
  int atmos;
  double red,green,blue;
  double radius,phase,phase0;
  double dradius,dphase1,dphase2;
  double rad_pos,theta_pos;

} hotspotstruc;

typedef struct
{

  int ntheta,nphi;
  double *x,*y,*z;

} spherestruc;
  
typedef struct
{

  int n,ndraw,ntheta;
  double red1,green1,blue1,red2,green2,blue2;
  double t,r1,r2;
  double limba,limbb;
  double *xc,*yc,*zc;
  double *x,*y,*z;
  double *xnormal,*ynormal,*znormal;
  double *red,*green,*blue,*op;

} streamstruc;

typedef struct
{

  int n;
  double *x,*y,*z,*r;
  double *red,*green,*blue;

} starfieldstruc;

  void init_all(void);
  void free_all(void);

  void init_sys(void);

  void init_sphere(void);
  void calc_sphere(void);
  void free_sphere(void);

  void init_disc(void);
  void calc_disc(void);
  void calc_spotpos(void);
  void calc_hotspot(void);
  void draw_disc(void);
  void free_disc(void);

  void init_stream(void);
  void calc_stream(void);
  void draw_stream(double dx,double dy,double dz);
  void free_stream(void);

  void init_starfield(void);
  void calc_starfield(void);
  void free_starfield(void);

  void unit_vector(double *a,double *b);
  double calc_rdisc2(double beta);
  void draw_sphere(double radius, double red, double green, double blue,
		   double slimba, double slimbb,
		   double op, double oprnd,
		   double r0, double rmin,
		   double dx, double dy, double dz);
  void render(void);
       
#ifdef __cplusplus
}
#endif
#endif
