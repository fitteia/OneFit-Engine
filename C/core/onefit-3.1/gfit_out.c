/******************************************************************************/
/*                             GFIT_OUTGRAF.C                                 */
/******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
/* #include <processes.h> */
#include <string.h>
#include "globals.h"

void    gfit_outgraf(par)
double  par[];
{
  int     i,k,xi,ma,mode,nxy;
  
  char *typex;
  char aux1[40];
  double  *fs,f,ft,*x;
  double  *dvector();
  
  void    free_dvector(),free_vector(),funcsn();
  
  FILE    *openf(),*fptr;
  
  fs = dvector(1,Ma);
  ma = Ma;
  
  for(i=1;i<=NT;i++) {
    nxy = num_gph_points[i-1];
    x  = dvector(0,nxy-1); 

    if(gph_name == NULL){
      if(_T!=NULL) sprintf(aux1,"gfitn%g.gph",_T[i]);
      else strcpy(aux1,"gfitn.gph");
      fptr = openf(aux1,"w");
    }
    else fptr = openf(gph_name[i-1],"w");

    if(xyt_flag){
      fprintf(fptr,"#Curvas teoricas para %s e T=%g K\n",dfiles[i-1],_T[i]);
/*      printf("#Curvas teoricas para %s e T=%lg K\n",dfiles[i-1],_T[i]);*/
    }
    else if(xy_flag){
      fprintf(fptr,"#Curvas teoricas para %s\n",dfiles[i-1]);
/*      printf("#Curvas teoricas para %s\n",dfiles[i-1]);*/
    }
    else nrerror("case not define in gfit_outgraf");

    typex = xscale[i-1];
    for(xi=0;xi<nxy;xi++) {
      f = (xM[i-1]-xm[i-1])*( (double) xi )/( ((double) nxy) -1.0)+xm[i-1];
      if(!strncmp(typex,"log",3))
	f = pow(10.0,f);
      else if(!strncmp(typex,"sqrt",4))
	f = pow(f,2.0);
      else f=f;
      x[xi] = f;

fprintf(fptr,"%lg ", x[xi]);
      
      mode=0;

      if(_T!=NULL) funcsn(x[xi],fs,ma,par,_T[i],mode);
      else funcsn(x[xi],fs,ma,par,1.0,mode);

      for(k=1,ft=0.0;k<=Ma;k++) {
	f = AA[k][i]*fs[k];
	ft += f;
	fprintf(fptr,"%e ",f);
      }
      fprintf(fptr,"%e ",ft);
      fprintf(fptr,"\n");
    }
    fclose(fptr);
    free_dvector(x,0,nxy-1);  
  }
  free_dvector(fs,1,Ma);
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
