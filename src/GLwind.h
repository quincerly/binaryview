#ifndef __GLWIND
#define __GLWIND

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

  double r,red,green,blue,op;
  double x,y,z;

} windstruc;
  
  void init_wind(windstruc *wind, double x, double y, double z);
  void draw_wind(windstruc *wind, double dx,double dy,double dz);
  double opwindfunc(double r);
 
#ifdef __cplusplus
}
#endif

#endif
