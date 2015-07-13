#ifndef __GLSTARFIELD
#define __GLSTARFIELD

#ifndef __DRAWBINARY
#include "drawbinary.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct
  {
    
    int n;
    double *x,*y,*z,*r;
    double *red,*green,*blue;
    
  } starfieldstruc;

  void init_starfield(starfieldstruc *starfield, systemparsstruc *sys);
  void calc_starfield(starfieldstruc *starfield, systemparsstruc *sys);
  void draw_starfield(starfieldstruc *starfield);
  void free_starfield(starfieldstruc *starfield);

#ifdef __cplusplus
}
#endif

#endif
