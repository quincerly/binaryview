int initpars(FILE *configfile)
{

  if ((getpar(configfile,"q",'D',&q)==0)||
      (getpar(configfile,"inc",'D',&incdeg)==0)||
      (getpar(configfile,"discradius1",'D',&discradius1)==0)||
      (getpar(configfile,"adisc",'D',&adisc)==0)||
      (getpar(configfile,"h",'D',&discthickness)==0)||
      (getpar(configfile,"e",'D',&edisc)==0)||
      (getpar(configfile,"phi0",'D',&discphase0)==0)||
      (getpar(configfile,"epsilon",'D',&epsilon)==0)||
      (getpar(configfile,"M1",'D',&M1)==0)||
      (getpar(configfile,"porb",'D',&Porb)==0)||
      (getpar(configfile,"pb",'D',&Pb)==0)||
      (getpar(configfile,"k0",'D',&k0)==0)||
      (getpar(configfile,"tstream",'D',&tstream)==0)||
      (getpar(configfile,"rstream1",'D',&streamr1)==0)||
      (getpar(configfile,"rstream2",'D',&streamr2)==0)) {
    return 0;
  }

  if (getpar(configfile,"nstream",'I',&nstream)==0) {
    nstream=100;
    //    printf("Default nstream = %d\n",nstream);
  }
  if (getpar(configfile,"nthetadonor",'I',&ntheta)==0) {
    ntheta=50;
    //    printf("Default nthetadonor = %d\n",ntheta);
  }
  if (getpar(configfile,"nphidonor",'I',&nphi)==0) {
    nphi=20;
    //    printf("Default nphidonor = %d\n",nphi);
  }
  if (getpar(configfile,"nrdisc",'I',&nrdisc)==0) {
    nrdisc=30;
    //    printf("Default nrdisc = %d\n",nrdisc);
  }
  if (getpar(configfile,"nphidisc",'I',&nbeta)==0) {
    nbeta=50;
    //    printf("Default nphidisc = %d\n",nbeta);
  }
  if (getpar(configfile,"limba",'D',&limba)==0) {
    limba=0.2;
    //    printf("Default limba = %g\n",limba);
  }
  if (getpar(configfile,"limbb",'D',&limbb)==0) {
    limbb=0.8;
    //    printf("Default limbb = %g\n",limbb);
  }
  if (getpar(configfile,"reddonor1",'D',&reddonor1)==0) {
    reddonor1=1.;
    //    printf("Default reddonor1 = %g\n",reddonor1);
  }
  if (getpar(configfile,"greendonor1",'D',&greendonor1)==0) {
    greendonor1=1.;
    //    printf("Default greendonor1 = %g\n",greendonor1);
  }
  if (getpar(configfile,"bluedonor1",'D',&bluedonor1)==0) {
    bluedonor1=0.;
    //    printf("Default bluedonor1 = %g\n",bluedonor1);
  }
  if (getpar(configfile,"reddonor2",'D',&reddonor2)==0) {
    reddonor2=1.;
    //    printf("Default reddonor2 = %g\n",reddonor2);
  }
  if (getpar(configfile,"greendonor2",'D',&greendonor2)==0) {
    greendonor2=0.;
    //    printf("Default greendonor2 = %g\n",greendonor2);
  }
  if (getpar(configfile,"bluedonor2",'D',&bluedonor2)==0) {
    bluedonor2=0.;
    //    printf("Default bluedonor2 = %g\n",bluedonor2);
  }
  if (getpar(configfile,"roughdonor",'D',&roughdonor)==0) {
    roughdonor=0.05;
    //    printf("Default roughdonor = %g\n",roughdonor);
  }
  if (getpar(configfile,"reddisc1",'D',&reddisc1)==0) {
    reddisc1=0.9;
    //    printf("Default reddisc1 = %g\n",reddisc1);
  }
  if (getpar(configfile,"greendisc1",'D',&greendisc1)==0) {
    greendisc1=0.8;
    //    printf("Default greendisc1 = %g\n",greendisc1);
  }
  if (getpar(configfile,"bluedisc1",'D',&bluedisc1)==0) {
    bluedisc1=1.;
    //    printf("Default bluedisc1 = %g\n",bluedisc1);
  }
  if (getpar(configfile,"reddisc2",'D',&reddisc2)==0) {
    reddisc2=0.7;
    //    printf("Default reddisc2 = %g\n",reddisc2);
  }
  if (getpar(configfile,"greendisc2",'D',&greendisc2)==0) {
    greendisc2=0.2;
    //    printf("Default greendisc2 = %g\n",greendisc2);
  }
  if (getpar(configfile,"bluedisc2",'D',&bluedisc2)==0) {
    bluedisc2=0.2;
    //    printf("Default bluedisc2 = %g\n",bluedisc2);
  }
  if (getpar(configfile,"redstream1",'D',&redstream1)==0) {
    redstream1=0.9;
    //    printf("Default redstream1 = %g\n",redstream1);
  }
  if (getpar(configfile,"greenstream1",'D',&greenstream1)==0) {
    greenstream1=0.;
    //    printf("Default greenstream1 = %g\n",greenstream1);
  }
  if (getpar(configfile,"bluestream1",'D',&bluestream1)==0) {
    bluestream1=0.;
    //    printf("Default bluestream1 = %g\n",bluestream1);
  }
  if (getpar(configfile,"redstream2",'D',&redstream2)==0) {
    redstream2=0.9;
    //    printf("Default redstream2 = %g\n",redstream2);
  }
  if (getpar(configfile,"greenstream2",'D',&greenstream2)==0) {
    greenstream2=0.5;
    //    printf("Default greenstream2 = %g\n",greenstream2);
  }
  if (getpar(configfile,"bluestream2",'D',&bluestream2)==0) {
    bluestream2=0.;
    //    printf("Default bluestream2 = %g\n",bluestream2);
  }
  if (getpar(configfile,"hotspotradius",'D',&hotspotradius)==0) {
    hotspotradius=0.8;
    //    printf("Default hotspotradius = %g\n",hotspotradius);
  }
  if (getpar(configfile,"hotspotphase",'D',&hotspotphase0)==0) {
    hotspotphase0=0.05;
    //    printf("Default hotspotphase = %g\n",hotspotphase0);
  }
  if (getpar(configfile,"nstarfield",'I',&nstarfield)==0) {
    nstarfield=500;
    //    printf("Default nstarfield = %d\n",nstarfield);
  }

  return 1;

}
