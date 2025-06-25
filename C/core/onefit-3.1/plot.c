#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "plot.h"
#include "funcsn.h"

void plotfitn()
{
  int nlinhas,*nchar_linha,pos=0;
  char **linhas;
  int npar,i,k;
  double *par;
  FILE *fitout;
  /*
    printf("I am in plotfitn.c\n");
  */
  if(res_output != NULL) fitout = openf(res_output,"r");
  else fitout = openf("gfitn.out","r");;

  nchar_linha = Flc(fitout,&nlinhas); /* vector with n of chars per line */
  /*
    puts("li fitout");
  */
 
  linhas = lines(fitout,nlinhas,nchar_linha);
  /*
    puts("li linhas");
  */
  pos = 1;

  if(strcmp(linhas[pos],"no parfile used")!=0) {
      /*
	puts("passei aqui");
      */
      npar  = (int) lers_num(linhas[pos]);
      par   = dvector(0,npar);
      par[0]= (double) npar;
      /*      printf("par[0]=%g\n",par[i]);*/
      pos = 2;
      for(i=1;i<par[0];i++) {
	  par[i]=lers_num(linhas[pos++]);

	  /*printf("par[%d]=%g\n",i,par[i]);*/
      }
  }
  else {
    par = NULL;
    pos++;
  }
  /*
    puts("li par");
  */
  for(k=1;k<=NT;k++) {
    if(linhas[pos][0] == 'T'){
      _T[k]  = lers_num(linhas[pos]);
    }
    else _T = NULL;
    pos++;
    for(i=1;i<=Ma;i++) {
      AA[i][k] = lers_num(linhas[pos]);
      pos++;
    }
    pos += 1+Ma;
  }
  plotgraf(par); 
  
  fclose(fitout);
}

/*****************************************************************************/
/*                                */
/*****************************************************************************/
void plotgraf(double *par)
// double  *par;
{
  int     i,k,xi,ma,mode=0,nxy;
  
  char *typex;
  char aux1[100];
  double  *fs,f,ft,*x;
  
  FILE    *fptr;
  
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
/**
printf("abri a file de gph\n");
**/
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
    /*
      puts(typex);
    */
    for(xi=0;xi<nxy;xi++) {
      f = (xM[i-1]-xm[i-1])*(double)xi/(nxy-1.0)+xm[i-1];
/**
printf("f=%g\n",f);
**/
      if(!strncmp(typex,"log",3))
	f = pow(10.0,f);
      else if(!strncmp(typex,"sqrt",4))
	f = pow(f,2.0);
      else f=f;
/**
printf("f=%g\n",f);
printf("xi=%d\n",xi);
**/
      x[xi] = f;
/**
printf("x=%g\n",x[xi]);
**/
      fprintf(fptr,"%g ",x[xi]);
/**
printf("x=%g ",x[xi]);
**/
      mode = 0;
/**
printf("x=%g ma=%d par[0]=%g mode=%d\n",x[xi],ma,par[0],mode);
**/
      if(_T!=NULL) funcsn(x[xi],fs,ma,par,_T[i],mode);
      else {
/**
printf("x=%g ma=%d par[0]=%g mode=%d\n",x[xi],ma,par[0],mode);
**/
	funcsn(x[xi],fs,ma,par,1.0,mode);
/**
printf("T=%s\n",_T);
**/
      }
      for(k=1,ft=0.0;k<=Ma;k++) {
	f = AA[k][i]*fs[k];
	ft += f;
	fprintf(fptr,"%e ",f);
/**
printf("f=%le ",f);
**/

      }
      fprintf(fptr,"%e ",ft);
      fprintf(fptr,"\n");
/**
printf("%le ",ft);
printf("\n");
**/
    }
    fclose(fptr);
    free_dvector(x,0,nxy-1);  
  }
  free_dvector(fs,1,Ma);
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
double lers_num(char *f)
// char *f;
{
   double   x;
   int i=0;

   while( f[i] != '=' && f[i]!= '\n') i++;

   sscanf(&f[i+1],"%lf",&x);
   return x;
}
/*****************************************************************************/
/*                                */
/*****************************************************************************/
