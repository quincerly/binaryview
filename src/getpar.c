//#include <stdio.h>
//#include "getpar.h"

int getpar(FILE *configfile, char *parname, char vartype, void *val) {
  
  char par[200],valuestring[200];
  int flag=0;
  double *dptr;
  int *iptr;
  long *lptr;
  char *sptr;

  rewind(configfile);

  while (!feof(configfile)&&(flag==0)) {
    fscanf(configfile,"%s %s",par,valuestring);
   if (strcmp(parname,par)==0) {
      flag=1;
      //printf("`%s'\n",valuestring);
      switch(vartype)
	{
	case 'D':
	  //printf("`%s' `%s'\n",par,valuestring);
	  dptr=(double *)val;
	  *dptr=atof(valuestring);
	  //printf("%s = %g\n",par,*dptr);
	  break;
	case 'I':
	  //printf("`%s' `%s'\n",par,valuestring);
	  iptr=(int *)val;
	  *iptr=atoi(valuestring);
	  //printf("%s = %d\n",par,*iptr);
	  break;
	case 'L':
	  //printf("`%s' `%s'\n",par,valuestring);
	  lptr=(long *)val;
	  *lptr=atol(valuestring);
	  //printf("%s = %ld\n",par,*lptr);
	  break;
	case 'S':
	  //printf("`%s' `%s'\n",par,valuestring);
	  sptr=(char *)val;
	  strcpy(sptr,valuestring);
	  //printf("%s = %s\n",par,sptr);
	  break;
	}
    }
  }

  return(flag);

}
