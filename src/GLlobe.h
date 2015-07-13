#ifndef __GLLOBE
#define __GLLOBE

#ifndef __DRAWBINARY
#include "drawbinary.h"
#endif
#ifndef __BINARYANIM
#include "binaryanim.h"
#endif
#ifndef __GLWIND
#include "GLwind.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
   
typedef struct
{

  int which, cutaway;
  int ntheta,nphi;
  double *x,*y,*z,*r;
  double *red,*green,*blue,*irr;
  double red1,green1,blue1,red2,green2,blue2;
  double limba,limbb;
  double rough,rfrac;
  double xirr,yirr,zirr,nxirr,nyirr,nzirr;
  double redirr,greenirr,blueirr,intirr,flareirr,dflareirr;

} lobestruc;

void init_lobe(lobestruc *lobe, int which, systemparsstruc *sys);
void calc_lobe(lobestruc *lobe, systemparsstruc *sys);
void draw_lobe(lobestruc *lobe, double dx,double dy,double dz, systemparsstruc *sys, controlstruc *ctl);
void free_lobe(lobestruc *lobe);
void cutawaycolour(double *rgb, int face, double r, double time);

#ifdef __cplusplus
}
#endif

#endif
