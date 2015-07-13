#ifndef __GLSPHERE
#define __GLSPHERE

#ifndef __DRAWBINARY
#include "drawbinary.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct
  {
    
    int ntheta,nphi;
    double *x,*y,*z;
    
  } spherestruc;
  
  void init_sphere(spherestruc *sphere);
  void calc_sphere(spherestruc *sphere);
  void free_sphere(spherestruc *sphere);
  void draw_sphere(spherestruc *sphere,
		   double radius, double red, double green, double blue,
		   double slimba, double slimbb,
		   double op, double oprnd,
		   double r0, double rmin,
		   double dx, double dy, double dz);

#ifdef __cplusplus
}
#endif
#endif
