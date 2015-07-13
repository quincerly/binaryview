/*************************************************/
/* Renders and animates close binary stars using */
/* Mesa3D under Linux.                           */
/* (C) 1999 Daniel Rolfe.                        */
/* Requires Mesa3D and libbinary                 */
/*************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include <misc.h>
#include <binary.h>
#include <stream.h>
#include <visibility.h>
#include "binaryanim.h"
#include "drawbinary.h"
#include "getpar.h"
#include "tiffio.h"
#include "writetiff.h"
#include "GLps.h"
#include "GLlobe.h"
#include "GLwind.h"
#include "GLjet.h"

#ifdef _USEGTK2
#include <gtk/gtk.h>
#endif

#define MINSCALE 0.1
#define MAXSCALE 10

controlstruc ctl;
systemparsstruc sys;
lobestruc donor;
lobestruc accretor;
discstruc disc;
hotspotstruc spot;
streamstruc stream;
starfieldstruc starfield;
spherestruc sphere;
windstruc donorwind;
windstruc accretorwind;
jetstruc accretorjet;

int initpars(char *filename);
void reshape(int width, int height);
void display(void);
void loadnew(char *filename);
void mainmenu(int value);
void annotatemenu(int value);
void configmenu(int value);
void writemenu(int value);
void centremenu(int value);
//void motion(int x, int y);
void keyboard(char key, int x, int y);
void specialkeyboard(int key, int x, int y);
void glut_post_redisplay_p(void);
void GLprintf(float x, float y, char *s, int size);
void updatemenus(void);

#ifdef _USEGTK2
void fileselector(char *text, char *defspec, void *callfn);
static void filesel_load_config(GtkWidget *w,GtkFileSelection *fs);
static void filesel_write_tiff(GtkWidget *w,GtkFileSelection *fs);
#endif

int mainmenid,annotatemenid,configmenid,writemenid,centremenid;

int main(int argc, char **argv)
{
  
  char windowtitle[120];
  FILE *configfile;
  
#ifdef _USEGTK2
  gtk_init (&argc, &argv);
#endif

  if (argc!=2) {
    printf("Usage : binaryanim configfile\n\n");
    exit(0);
  }

  strcpy(windowtitle,"BINARYVIEW ");
  strcat(windowtitle,argv[1]);
  
  if (strlen(argv[1])>FILENAMELEN) {
    printf("!!! config filename too long.\n");
    exit(0);
  }

  if (initpars(argv[1])==0) exit(0);

  init_all();
  
  glutInit(&argc, argv);
  glutInitDisplayMode ( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize (ctl.window_width, ctl.window_height);
  glutCreateWindow (windowtitle);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);

  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutIdleFunc(glut_post_redisplay_p);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialkeyboard);

  // Create annotate menu
  annotatemenid=glutCreateMenu(annotatemenu);
  glutAddMenuEntry("Annotation off", 1);
  glutAddMenuEntry("Font size 10", 2);
  glutAddMenuEntry("Font size 18", 3);

  // Create configuration menu
  configmenid=glutCreateMenu(configmenu);
  glutAddMenuEntry("Reload", 1);
#ifdef _USEGTK2
  glutAddMenuEntry("Load new", 2);
#endif

  // Create output menu
  writemenid=glutCreateMenu(writemenu);
  glutAddMenuEntry("Write TIFF image", 1);
  glutAddMenuEntry("Write Postscript image", 2);

  // Create centre menu
  centremenid=glutCreateMenu(centremenu);
  glutAddMenuEntry("Centre of mass", 1);
  glutAddMenuEntry("Compact object", 2);
  glutAddMenuEntry("Donor star", 3);
    
  // Create main menu
  mainmenid=glutCreateMenu(mainmenu);
  if (ctl.paused==0) glutAddMenuEntry("Pause animation",1); else glutAddMenuEntry("Start animation", 1);
  glutAddSubMenu("Centre on .. ", centremenid);
  glutAddSubMenu("Annotate", annotatemenid);
  if (ctl.fullscreen==0) glutAddMenuEntry("Fullscreen mode",4); else glutAddMenuEntry("Fullscreen mode off",4);
  glutAddSubMenu("Configuration", configmenid);
  glutAddSubMenu("Write", writemenid);
  glutAddMenuEntry("Quit", 7);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glViewport(0, 0, ctl.window_width, ctl.window_height);

  if (ctl.fullscreen==1) glutFullScreen();

  glutMainLoop();

  free_all();
  return 0;

}

int initpars(char *filename)
{
  
  FILE *configfile;

  if (filename) strcpy(ctl.configfilename,filename);

  if (!(configfile=fopen(ctl.configfilename,"r"))) {
    printf("Couldn't open configuration `%s'.\n",ctl.configfilename);
    exit(0);
  }

  if ((getpar(configfile,"q",'D',&sys.q)==0)||
      (getpar(configfile,"inc",'D',&sys.incdeg)==0)||
      (getpar(configfile,"periodsec",'D',&sys.periodsec)==0)||
      (getpar(configfile,"discradius1",'D',&disc.radius1)==0)||
      (getpar(configfile,"adisc",'D',&disc.a)==0)||
      (getpar(configfile,"h",'D',&disc.thickness)==0)||
      (getpar(configfile,"e",'D',&disc.e)==0)||
      (getpar(configfile,"phi0",'D',&disc.shphase0)==0)||
      (getpar(configfile,"epsilon",'D',&sys.epsilon)==0)||
      (getpar(configfile,"M1",'D',&sys.M1)==0)||
      (getpar(configfile,"porb",'D',&sys.Porb)==0)||
      (getpar(configfile,"pb",'D',&sys.Pb)==0)||
      (getpar(configfile,"k0",'D',&sys.k0)==0)||
      (getpar(configfile,"tstream",'D',&stream.t)==0)||
      (getpar(configfile,"rstream1",'D',&stream.r1)==0)||
      (getpar(configfile,"rstream2",'D',&stream.r2)==0)) {
    fclose(configfile);
    return 0;
  }

  if (getpar(configfile,"tiffname",'S',&ctl.tiffname)==0) strcpy(ctl.tiffname,"binary.tiff");
  if (getpar(configfile,"epsname",'S',&ctl.epsname)==0) strcpy(ctl.epsname,"binary.ps");
  if (getpar(configfile,"scale",'D',&ctl.scale)==0) ctl.scale=1;
  if (getpar(configfile,"clear",'I',&ctl.cleartoblack)==0) ctl.cleartoblack=0;
  if (getpar(configfile,"fullscreen",'I',&ctl.fullscreen)==0) ctl.fullscreen=0;
  if (getpar(configfile,"width",'I',&ctl.window_width0)==0) ctl.window_width0=350;
  if (getpar(configfile,"height",'I',&ctl.window_height0)==0) ctl.window_height0=350;
  if (getpar(configfile,"stream",'I',&ctl.streamflag)==0) ctl.streamflag=1;
  if (getpar(configfile,"teapot",'I',&ctl.teapot)==0) ctl.teapot=0;
  if (getpar(configfile,"paused",'I',&ctl.paused)==0) ctl.paused=0;
  if (getpar(configfile,"annotate",'I',&ctl.annotate)==0) ctl.annotate=10;
  if (getpar(configfile,"frames",'I',&ctl.frames)==0) ctl.frames=0;
  else if (getpar(configfile,"framedphi",'D',&ctl.framedphi)==0) ctl.framedphi=1./(double)ctl.frames;

  if (getpar(configfile,"orbphase",'D',&sys.orbphase0)==0) sys.orbphase0=0.;

  if (getpar(configfile,"redhotspot",'D',&spot.red)==0) spot.red=1.;
  if (getpar(configfile,"greenhotspot",'D',&spot.green)==0) spot.green=0.9;
  if (getpar(configfile,"bluehotspot",'D',&spot.blue)==0) spot.blue=1.;
  if (getpar(configfile,"spotatmos",'I',&spot.atmos)==0) spot.atmos=1.;

  if (getpar(configfile,"nthetadonor",'I',&donor.ntheta)==0) donor.ntheta=50;
  if (getpar(configfile,"nphidonor",'I',&donor.nphi)==0) donor.nphi=20;
  if (getpar(configfile,"limbadonor",'D',&donor.limba)==0) donor.limba=0.2;
  if (getpar(configfile,"limbbdonor",'D',&donor.limbb)==0) donor.limbb=0.8;
  if (getpar(configfile,"reddonor1",'D',&donor.red1)==0) donor.red1=1.;
  if (getpar(configfile,"greendonor1",'D',&donor.green1)==0) donor.green1=1.;
  if (getpar(configfile,"bluedonor1",'D',&donor.blue1)==0) donor.blue1=0.;
  if (getpar(configfile,"reddonor2",'D',&donor.red2)==0) donor.red2=1.;
  if (getpar(configfile,"greendonor2",'D',&donor.green2)==0) donor.green2=0.;
  if (getpar(configfile,"bluedonor2",'D',&donor.blue2)==0) donor.blue2=0.;
  if (getpar(configfile,"roughdonor",'D',&donor.rough)==0) donor.rough=0.05;
  if (getpar(configfile,"rfracdonor",'D',&donor.rfrac)==0) donor.rfrac=1.;
  if (getpar(configfile,"cutdonor",'I',&donor.cutaway)==0) donor.cutaway=0;
  if (getpar(configfile,"redirrdonor",'D',&donor.redirr)==0) donor.redirr=1.;
  if (getpar(configfile,"greenirrdonor",'D',&donor.greenirr)==0) donor.greenirr=1.;
  if (getpar(configfile,"blueirrdonor",'D',&donor.blueirr)==0) donor.blueirr=1.;
  if (getpar(configfile,"intirrdonor",'D',&donor.intirr)==0) donor.intirr=0.;
  if (getpar(configfile,"flareirrdonor",'D',&donor.flareirr)==0) donor.flareirr=0.;
  if (getpar(configfile,"dflareirrdonor",'D',&donor.dflareirr)==0) donor.dflareirr=0.;

  if (getpar(configfile,"rwinddonor",'D',&donorwind.r)==0) donorwind.r=0.;
  if (getpar(configfile,"redwinddonor",'D',&donorwind.red)==0) donorwind.red=1.;
  if (getpar(configfile,"greenwinddonor",'D',&donorwind.green)==0) donorwind.green=1.;
  if (getpar(configfile,"bluewinddonor",'D',&donorwind.blue)==0) donorwind.blue=1.;
  if (getpar(configfile,"opwinddonor",'D',&donorwind.op)==0) donorwind.op=0.7;

  if (getpar(configfile,"nthetaaccretor",'I',&accretor.ntheta)==0) accretor.ntheta=50;
  if (getpar(configfile,"nphiaccretor",'I',&accretor.nphi)==0) accretor.nphi=20;
  if (getpar(configfile,"limbaaccretor",'D',&accretor.limba)==0) accretor.limba=0.2;
  if (getpar(configfile,"limbbaccretor",'D',&accretor.limbb)==0) accretor.limbb=0.8;
  if (getpar(configfile,"redaccretor1",'D',&accretor.red1)==0) accretor.red1=1.;
  if (getpar(configfile,"greenaccretor1",'D',&accretor.green1)==0) accretor.green1=1.;
  if (getpar(configfile,"blueaccretor1",'D',&accretor.blue1)==0) accretor.blue1=0.;
  if (getpar(configfile,"redaccretor2",'D',&accretor.red2)==0) accretor.red2=1.;
  if (getpar(configfile,"greenaccretor2",'D',&accretor.green2)==0) accretor.green2=0.;
  if (getpar(configfile,"blueaccretor2",'D',&accretor.blue2)==0) accretor.blue2=0.;
  if (getpar(configfile,"roughaccretor",'D',&accretor.rough)==0) accretor.rough=0.05;
  if (getpar(configfile,"rfracaccretor",'D',&accretor.rfrac)==0) accretor.rfrac=1.;
  if (getpar(configfile,"cutaccretor",'I',&accretor.cutaway)==0) accretor.cutaway=0;
  if (getpar(configfile,"redirraccretor",'D',&accretor.redirr)==0) accretor.redirr=1.;
  if (getpar(configfile,"greenirraccretor",'D',&accretor.greenirr)==0) accretor.greenirr=1.;
  if (getpar(configfile,"blueirraccretor",'D',&accretor.blueirr)==0) accretor.blueirr=1.;
  if (getpar(configfile,"intirraccretor",'D',&accretor.intirr)==0) accretor.intirr=0.;
  if (getpar(configfile,"flareirraccretor",'D',&accretor.flareirr)==0) accretor.flareirr=0.;
  if (getpar(configfile,"dflareirraccretor",'D',&accretor.dflareirr)==0) accretor.dflareirr=0.;

  if (getpar(configfile,"rwindaccretor",'D',&accretorwind.r)==0) accretorwind.r=0.;
  if (getpar(configfile,"redwindaccretor",'D',&accretorwind.red)==0) accretorwind.red=1.;
  if (getpar(configfile,"greenwindaccretor",'D',&accretorwind.green)==0) accretorwind.green=1.;
  if (getpar(configfile,"bluewindaccretor",'D',&accretorwind.blue)==0) accretorwind.blue=1.;
  if (getpar(configfile,"opwindaccretor",'D',&accretorwind.op)==0) accretorwind.op=0.7;

  if (getpar(configfile,"nrdisc",'I',&disc.nr)==0) disc.nr=30;
  if (getpar(configfile,"nphidisc",'I',&disc.nbeta)==0) disc.nbeta=50;
  if (getpar(configfile,"reddisc1",'D',&disc.red1)==0) disc.red1=0.9;
  if (getpar(configfile,"greendisc1",'D',&disc.green1)==0) disc.green1=0.8;
  if (getpar(configfile,"bluedisc1",'D',&disc.blue1)==0) disc.blue1=1.;
  if (getpar(configfile,"reddisc2",'D',&disc.red2)==0) disc.red2=0.7;
  if (getpar(configfile,"greendisc2",'D',&disc.green2)==0) disc.green2=0.2;
  if (getpar(configfile,"bluedisc2",'D',&disc.blue2)==0) disc.blue2=0.2;
  if (getpar(configfile,"opdisc1",'D',&disc.op1)==0) disc.op1=0.8;
  if (getpar(configfile,"opdisc2",'D',&disc.op2)==0) disc.op2=0.2;

  if (getpar(configfile,"nzjet",'I',&accretorjet.nz)==0) accretorjet.nz=50;
  if (getpar(configfile,"nphijet",'I',&accretorjet.nphi)==0) accretorjet.nphi=30;
  if (getpar(configfile,"redjet1",'D',&accretorjet.red1)==0) accretorjet.red1=0.3;
  if (getpar(configfile,"greenjet1",'D',&accretorjet.green1)==0) accretorjet.green1=0.8;
  if (getpar(configfile,"bluejet1",'D',&accretorjet.blue1)==0) accretorjet.blue1=0.3;
  if (getpar(configfile,"redjet2",'D',&accretorjet.red2)==0) accretorjet.red2=0.3;
  if (getpar(configfile,"greenjet2",'D',&accretorjet.green2)==0) accretorjet.green2=0.8;
  if (getpar(configfile,"bluejet2",'D',&accretorjet.blue2)==0) accretorjet.blue2=0.3;
  if (getpar(configfile,"opjet1",'D',&accretorjet.op1)==0) accretorjet.op1=0.2;
  if (getpar(configfile,"opjet2",'D',&accretorjet.op2)==0) accretorjet.op2=0.2;
  if (getpar(configfile,"lengthjet",'D',&accretorjet.length)==0) accretorjet.length=0.;
  if (getpar(configfile,"rjet",'D',&accretorjet.radius)==0) accretorjet.radius=0.03;
  if (getpar(configfile,"roundfjet",'D',&accretorjet.rf)==0) accretorjet.rf=0.1;
  if (getpar(configfile,"limbajet",'D',&accretorjet.limba)==0) accretorjet.limba=0.;
  if (getpar(configfile,"limbbjet",'D',&accretorjet.limbb)==0) accretorjet.limbb=1.;

  if (getpar(configfile,"nstream",'I',&stream.n)==0) stream.n=100;
  if (getpar(configfile,"limbastream",'D',&stream.limba)==0) stream.limba=0.2;
  if (getpar(configfile,"limbbstream",'D',&stream.limbb)==0) stream.limbb=0.8;
  if (getpar(configfile,"redstream1",'D',&stream.red1)==0) stream.red1=0.9;
  if (getpar(configfile,"greenstream1",'D',&stream.green1)==0) stream.green1=0.;
  if (getpar(configfile,"bluestream1",'D',&stream.blue1)==0) stream.blue1=0.;
  if (getpar(configfile,"redstream2",'D',&stream.red2)==0) stream.red2=0.9;
  if (getpar(configfile,"greenstream2",'D',&stream.green2)==0) stream.green2=0.5;
  if (getpar(configfile,"bluestream2",'D',&stream.blue2)==0) stream.blue2=0.;

  if (getpar(configfile,"hotspotradius",'D',&spot.radius)==0) spot.radius=0.95;
  if (getpar(configfile,"hotspotphase",'D',&spot.phase0)==0) spot.phase0=0.04;
  if (getpar(configfile,"hotspotdradius",'D',&spot.dradius)==0) spot.dradius=0.03;
  if (getpar(configfile,"hotspotdphase1",'D',&spot.dphase1)==0) spot.dphase1=0.01;
  if (getpar(configfile,"hotspotdphase2",'D',&spot.dphase2)==0) spot.dphase2=0.15;

  if (getpar(configfile,"nstarfield",'I',&starfield.n)==0) starfield.n=500;

  fclose(configfile);

  if (ctl.annotate<0) ctl.annotate=0;
  if (ctl.annotate>18) ctl.annotate=18;
  ctl.window_width=ctl.window_width0;
  ctl.window_height=ctl.window_height0;
  sys.shphase=disc.shphase0;
  spot.phase=spot.phase0;

  return 1;
  
}

void reshape(int width, int height) {

        // Set the new viewport size
        glViewport(0, 0, (GLint)width, (GLint)height);
        glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
        glOrtho(-1.3, 1.3, -1.3, 1.3, -10., 10.);
	glScaled(1.,(double)width/(double)height,1.);
 	glRotated(-sys.incdeg,1.,0.,0.);
	glMatrixMode(GL_MODELVIEW);

	ctl.window_width=width;
	ctl.window_height=height;

}

void display(void)
{

  double phasedegchange=0;
  char phaseincstring[100];
  char framename[104];

  // Calculate phases at this time
  if ((ctl.frames!=0)&&(ctl.frame<ctl.frames)) {
    sys.phasedeg=sys.orbphase0*360.+ctl.frame*ctl.framedphi*360.;
    sys.shphase=disc.shphase0+ctl.frame*ctl.framedphi/(1.+sys.epsilon);
  }
  else if (ctl.paused==0) {
    ctl.thistime=glutGet(GLUT_ELAPSED_TIME);
    phasedegchange=(double)(ctl.thistime-ctl.lasttime)*0.36/sys.periodsec;
    ctl.lasttime=ctl.thistime;
    sys.phasedeg+=phasedegchange;
    sys.shphase+=phasedegchange/360./(1.+sys.epsilon);
  }

  ctl.realtime=sys.phasedeg/360.*sys.Porb;
  sys.shphase=sys.shphase-(int)sys.shphase;

  // Clear the RGB buffer and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw it
  render();

  // Plot the
  if (ctl.annotate>0) {
    glDisable(GL_DEPTH_TEST);
    glColor4f(1.0,1.0,1.0,1.0);
    sprintf(phaseincstring,
	    "Inclination %4.1f. Orbital phase %5.3f",
	    sys.incdeg,fold(sys.phasedeg/360.));
    GLprintf(-1.+0.05*((float)ctl.window_height/(float)ctl.window_width),-0.95,phaseincstring,ctl.annotate);
    glEnable(GL_DEPTH_TEST);
  }

  // Swap the buffers
  glutSwapBuffers();

  // Save out animframe
  if ((ctl.frames!=0)&&(ctl.frame<ctl.frames)) {
    sprintf(framename,"%s.%04d.tiff",ctl.tiffname,ctl.frame);
    writetiff(framename, "OpenGL-rendered interacting binary star", 0, 0,
	      ctl.window_width,ctl.window_height, COMPRESSION_PACKBITS);
    printf("Wrote frame `%s'\n",framename);
    ctl.frame++;
  }  
}

void glut_post_redisplay_p(void)
{
  if (ctl.paused==0) glutPostRedisplay();
}

void keyboard(char key, int x, int y)
{
  if ((ctl.frames==0)||(ctl.frame>=ctl.frames)) {
    switch(key)
      {
	
      case 'a' :
	switch (ctl.annotate) {
	case 0: ctl.annotate=10;
	  break;
	case 10: ctl.annotate=18;
	  break;
	case 18: ctl.annotate=0;
	  break;
	default: ctl.annotate=0;
	  break;
	}
	glutPostRedisplay();
	break;
      case 'p' : // Pause
	if (ctl.paused==0) {
	  ctl.paused=1;
	  glutSetMenu(mainmenid);
	  glutChangeToMenuEntry(1,"Resume animation",1);
	}
	else {   // Unpause
	  ctl.paused=0;
	  ctl.lasttime=glutGet(GLUT_ELAPSED_TIME);
	  glutSetMenu(mainmenid);
	  glutChangeToMenuEntry(1,"Pause animation",1);
	}
	break;
      case '-' : // Zoom out
	ctl.scale=ctl.scale-0.05;
	if (ctl.scale<MINSCALE) ctl.scale=MINSCALE;
	glutPostRedisplay();
	break;
      case '=' : // Zoom in
	ctl.scale=ctl.scale+0.05;
	if (ctl.scale>MAXSCALE) ctl.scale=MAXSCALE;
	glutPostRedisplay();
	break;
      case 't' :
        if (ctl.teapot==0) ctl.teapot=1;
        else ctl.teapot=0;
        glutPostRedisplay();
        break;
      case 'c' : // Toggle donor cutaway
	if (donor.cutaway==0) donor.cutaway=1;
	else donor.cutaway=0;
	glutPostRedisplay();
	break;
      case 'C' : // Toggle accretor cutaway
	if (accretor.cutaway==0) accretor.cutaway=1;
	else accretor.cutaway=0;
	glutPostRedisplay();
	break;
      case '2' : // Centre on donor
	ctl.centre[0]=sys.x2;
	ctl.centre[1]=0.;
	ctl.centre[2]=0.;
	glutPostRedisplay();
	break;
      case '1' : // Centre on accretor
	ctl.centre[0]=sys.x1;
	ctl.centre[1]=0.;
	ctl.centre[2]=0.;
	glutPostRedisplay();
	break;
      case '0' : // Centre on centre of mass
	ctl.centre[0]=0.;
	ctl.centre[1]=0.;
	ctl.centre[2]=0.;
	glutPostRedisplay();
	break;
      case 'r' : // Reload
	loadnew(NULL);
	break;
      }
  }
}

void specialkeyboard(int key, int x, int y)
{
  double newincdeg;
  if ((ctl.frames==0)||(ctl.frame>=ctl.frames)) {
    switch(key) {
    case GLUT_KEY_UP:
      newincdeg=sys.incdeg+0.5;
      if ((newincdeg>=0.)&&(newincdeg<=90.)) {
	sys.incdeg=newincdeg;
	glutPostRedisplay();
      }
      break;
    case GLUT_KEY_DOWN:
      newincdeg=sys.incdeg-0.5;
      if ((newincdeg>=0.)&&(newincdeg<=90.)) {
	sys.incdeg=newincdeg;
	glutPostRedisplay();
      }
      break;
    case GLUT_KEY_RIGHT:
      if (ctl.paused==1) {
	sys.phasedeg+=0.5;
	sys.shphase+=0.5/360.*sys.epsilon/(1.+sys.epsilon);
	glutPostRedisplay();
      }
      break;
    case GLUT_KEY_LEFT:
      if (ctl.paused==1) {
	sys.phasedeg+=-0.5;
	sys.shphase+=-0.5/360.*sys.epsilon/(1.+sys.epsilon);
	glutPostRedisplay();
      }
      break;
    }
  }
}

void mainmenu(int value)
{
  if ((ctl.frames!=0)&&(ctl.frame<ctl.frames)) {
    if (value==7) exit(0);
  }  
  else {

    switch (value) {
    case 1:
      if (ctl.paused==0) {
	ctl.paused=1;
	glutSetMenu(mainmenid);
	glutChangeToMenuEntry(1,"Resume animation",1);
      }
      else {
	ctl.paused=0;
	ctl.lasttime=glutGet(GLUT_ELAPSED_TIME);
	glutSetMenu(mainmenid);
	glutChangeToMenuEntry(1,"Pause animation",1);
      }
      break;
    case 4:
      if (ctl.fullscreen==0) {
	ctl.window_width0=ctl.window_width;
	ctl.window_height0=ctl.window_height;
	glutChangeToMenuEntry(4,"Fullscreen mode off",4);
	glutFullScreen();
	ctl.fullscreen=1;
      } else {
	glutChangeToMenuEntry(4,"Fullscreen mode",4);
	glutReshapeWindow(ctl.window_width0,ctl.window_height0);
	ctl.fullscreen=0;
      }
      break;
    case 7:
      free_all();
      exit(0);
      break;
    }
  }
}

void annotatemenu(int value)
{

  switch (value) {
  case 1: ctl.annotate=0;
    break;
  case 2: ctl.annotate=10;
    break;
  case 3: ctl.annotate=18;
    break;
  }
  glutPostRedisplay();

}

void loadnew(char *filename)
{

  char windowtitle[120];

  free_all();
  if (initpars(filename)==0) exit(0);
  init_all();

  updatemenus();

  glutReshapeWindow(ctl.window_width0,ctl.window_height0);
  strcpy(windowtitle,"BINARYVIEW ");
  strcat(windowtitle,ctl.configfilename);
  glutSetWindowTitle(windowtitle);
  glutPostRedisplay();
}

void updatemenus(void)
// Make sure menus match current settings
{

  if (ctl.paused==0) {
    glutSetMenu(mainmenid);
    glutChangeToMenuEntry(1,"Pause animation",1);
  } else {
    glutSetMenu(mainmenid);
    glutChangeToMenuEntry(1,"Resume animation",1);
  }
  
  if (ctl.fullscreen==0) {
    glutSetMenu(mainmenid);
    glutChangeToMenuEntry(4,"Fullscreen mode",4);
  } else {
    glutSetMenu(mainmenid);
    glutChangeToMenuEntry(4,"Fullscreen mode off",4);
  }


}

void configmenu(int value)
// Handle configuration menu
{

  switch(value) {
  case 1:
    loadnew(NULL);
    break;
#ifdef _USEGTK2
  case 2:
    fileselector("Choose config file to load","configs/",filesel_load_config);
    break;
#endif
  }
}

#ifdef _USEGTK2
void fileselector(char *text, char *defspec, void (*callfn))
{

  GtkWidget *filew;

  filew = gtk_file_selection_new (text);
  
  g_signal_connect (G_OBJECT (filew), "destroy",
		    G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		    "clicked", G_CALLBACK (callfn), (gpointer) filew);
  g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
			    "clicked", G_CALLBACK (gtk_widget_destroy),
			    G_OBJECT (filew));
  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
				   defspec);
  
  gtk_widget_show (filew);
  gtk_main ();
}

static void filesel_load_config(GtkWidget *w,GtkFileSelection *fs)
// Load file selected in GTK2 file requester
{
  loadnew(gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
  gtk_widget_destroy(G_OBJECT (fs));
  gtk_main_quit();
}
static void filesel_write_tiff(GtkWidget *w,GtkFileSelection *fs)
// Load file selected in GTK2 file requester
{
  writetiff(gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)),
	    "OpenGL-rendered interacting binary star", 0, 0,
	    ctl.window_width,ctl.window_height, COMPRESSION_NONE);
  printf("Wrote '%s'.\n",gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));  
  gtk_widget_destroy(G_OBJECT (fs));
  gtk_main_quit();
}
#endif

void writemenu(int value)
{
  
  switch(value) {
  case 1:
#ifdef _USEGTK2
    fileselector("Select tiff to write as",ctl.tiffname,filesel_write_tiff);
#else
    writetiff(ctl.tiffname, "OpenGL-rendered interacting binary star", 0, 0,
	      ctl.window_width,ctl.window_height, COMPRESSION_NONE);
    printf("Wrote '%s'.\n",ctl.tiffname);
#endif
    break;
  case 2:
    ctl.transparent=0;
    glutPostRedisplay();
    outputEPS(ctl.objectComplexity, 1, ctl.epsname);
    printf("Wrote '%s'.\n",ctl.epsname);
    ctl.transparent=1;
    glutPostRedisplay();      
    break;
  }
}

void centremenu(int value)
{
  
  switch(value) {
      case 1:
	ctl.centre[0]=0.;
	ctl.centre[1]=0.;
	ctl.centre[2]=0.;
	glutPostRedisplay();
	break;
      case 2:
	ctl.centre[0]=sys.x1;
	ctl.centre[1]=0.;
	ctl.centre[2]=0.;
	glutPostRedisplay();
	break;
      case 3:
	ctl.centre[0]=sys.x2;
	ctl.centre[1]=0.;
	ctl.centre[2]=0.;
	glutPostRedisplay();
	break;
  }
}

void GLprintf(float x, float y, char *s, int size)
{ 

  int i; 

  glMatrixMode (GL_PROJECTION); 
  glLoadIdentity (); 
  glRasterPos2f(x,y); 

  switch(size)
    {
    case 10:
      for (i=0; i<strlen(s); i++) 
	glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]); 
      break;
    case 18:
      for (i=0; i<strlen(s); i++) 
	glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, s[i]); 
      break;
    default:
      for (i=0; i<strlen(s); i++) 
	glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]); 
      break;
    } 


}

#include "drawbinary.c"
#include "GLdisc.c"
#include "GLstream.c"
#include "getpar.c"
