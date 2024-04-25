/******************************************************************************/
/*                             GFIT_OUTPG.C                                   */
/******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
/* #include processes.h */
#include <string.h>
#include "globals.h"


void    gfit_outpg(par)
double  par[];
{
  int     i,k,xi,ma,mode;
  char    aux1[100];
  double  **Data,*fs,f,ft,*chisq,chi2t;
  double  Inv(),Iden(),*dvector();
  double   sqrt(),log10(),logT1(),gchi2_vn(),gchi2out();
  double  gammq(),QL,ntotdat=0,ntotpar=0,a1,a2;
  
  void    free_dvector(),free_vector(),pp3d(),ppcovar(),funcsn();
  
  FILE    *openf(),*fitout,*fptr;
  
  fs    = dvector(1,Ma);
  chisq = dvector(1,NT);
  ma    = Ma;
  
  if(res_output != NULL) fitout = openf(res_output,"w");
  else fitout = openf("gfitn.out","w");;

  for(i=1;i<=NT;i++) ntotdat += Ndata[i];
   ntotpar= FPar + NT*Mfit;
  chi2t = gchi2_vn(par,chisq);
  /*	for(i=1;i<=NT;i++) for(k=1;k<=Ma;k++) printf("AA[%d][%d]=%g\n",k,i,AA[k][i]);*/
  a1=0.5*(ntotdat - ntotpar);
  a2=0.5*chi2t;
  QL = gammq(a1, a2);
  /*  printf("#        y = %s\n",FUNCAO);*/
  fprintf(fitout,"#       y = %s\n",FUNCAO);
  /*  printf("chi2t=%g CL(%g,%g)=%g\n\n",chi2t,a1,a2,QL); */
  fprintf(fitout,"chi2t=%g CL(%g,%g)=%g\n\n",chi2t,a1,a2,QL);
  
  if(par != NULL) {
    for(i=0;i<=par[0]-1;i++) {
        fprintf(fitout,"%10s =%12.5e\n",ParNames[i],par[i]);
	/*	printf("%10s =%12.5e\n",ParNames[i],par[i]);*/
    }
    fprintf(fitout,"\n");
    /*    printf("\n");*/
  }
  else {
    fprintf(fitout,"no parfile used\n\n");
    /*    printf("no parfile used\n\n");*/
  }
  for(k=1;k<=NT;k++) {
    a1=0.5*(Ndata[k] - Mfit - FPar);
    a2=0.5*chisq[k];
    QL = gammq(a1, a2);
    if(xyt_flag) {
      /*      printf("T=%g ",_T[k]);*/
      fprintf(fitout,"T=%g ",_T[k]);
    }
    /*    printf("chi2[%d]=%g CL(%g,%g)=%g\n",k,chisq[k],a1,a2,QL);*/
    fprintf(fitout,"chi2[%d]=%g CL(%g,%g)=%g\n",k,chisq[k],a1,a2,QL);
    gchi2out(par,k,fitout);
  }
  fprintf(fitout,"\n\n");
  /*  printf("\n\n");*/
  
#ifndef LINUX
  if(FPar+NT*Mfit >0 ) ppcovar(fitout,par);
#endif

  /*  printf("\n");*/
  fprintf(fitout,"\n");
  
  mode = 0;

  
  for(i=1;i<=NT;i++) {
    Data = VData[i-1];
    if(_T!=NULL) sprintf(aux1,"fit-residues-%d.res",i);
    else strcpy(aux1,"fit-residues.res");
    fptr = openf(aux1,"w");
    if(xyt_flag){
      fprintf(fptr,"# Data and Theoretical values: exp_x exp_y theo_y diff\n");
      //      fprintf(fptr,"# Curvas para %s e T=%g K\n",dfiles[i-1],_T[i]);
    }
    else if(xy_flag){
      fprintf(fptr,"# Data and Theoretical values: exp_x exp_y theo_y diff\n");
    }
    else nrerror("case not defined in gfit_outpg");
    
    for(xi=1;xi<=Ndata[i];xi++) {
      f = Data[xi][1];
      /*      printf("%le\n",f);*/
      if(_T!=NULL) funcsn(f,fs,ma,par,_T[i],mode);
      else funcsn(f,fs,ma,par,1.0,mode);

      fprintf(fptr,"%le %le ",f,Data[xi][2]);
      
      for(k=1,ft=0.0;k<=Ma;k++) {
	f = AA[k][i]*fs[k];
	ft += f;
	/*	fprintf(fptr,"%le ",f); */
      }
      fprintf(fptr,"%le %le\n",ft,Data[xi][2]-ft);
    }
       fclose(fptr);
  }

  
  
  free_dvector(fs,1,Ma);
  free_dvector(chisq,1,NT);
  fclose(fitout);
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/

