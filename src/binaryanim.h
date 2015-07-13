#ifndef __BINARYANIM
#define __BINARYANIM
#ifdef __cplusplus
extern "C" {
#endif
  
#define FILENAMELEN 100

typedef struct
{

  // Basic view parameters
  int window_width;
  int window_height;
  int window_width0;
  int window_height0;
  int fullscreen;
  double scale;
  
  // Animation parameters
  int thistime;
  int lasttime;
  int paused;
  int frames;
  int frame;
  double framedphi;
  double realtime;

  // File stuff
  char configfilename[FILENAMELEN+1];
  char tiffname[FILENAMELEN+1];
  char epsname[FILENAMELEN+1];

  int objectComplexity;

  // Flags for various modes and options
  int streamflag;
  int teapot;
  int cutaway;
  int annotate;
  int transparent;  
  int cleartoblack;
  double centre[3];

} controlstruc;

#ifdef __cplusplus
}
#endif
#endif
