#ifndef __GLJET
#define __GLJET

#ifndef __DRAWBINARY
#include "drawbinary.h"
#endif
#ifndef __BINARYANIM
#include "binaryanim.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
   
typedef struct
{

  int nz,nphi;
  double *x,*y,*z;
  double *xnormal,*ynormal,*znormal;
  double *red,*green,*blue,*op;
  double red1,green1,blue1,red2,green2,blue2,op1,op2;
  double length,radius;
  double xcentre,ycentre,zcentre;
  double limba,limbb;
  double rf; // Fraction of length at which rounding begins
  double zpeak;

} jetstruc;

void init_jet(jetstruc *jet,double xcentre,double ycentre,double zcentre);
void calc_jet(jetstruc *jet);
void draw_jet(jetstruc *jet, char side, double dx,double dy,double dz, systemparsstruc *sys);
void free_jet(jetstruc *jet);
double jet_radius(double zfrac, double rf);
double djet_radius_dzf(double zfrac, double rf);

#ifdef __cplusplus
}
#endif

#endif
