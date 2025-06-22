#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#include "integra.h"

#define	pi	3.141592653589793238
#define PI 3.1415926
#define	C	0.569504e-12
#define LIM	3.0e-11

double GaussLegendreInt(Function *X,int p,int n)
{
  int m,j,i;
  double z1,z,xm,xl,pp,p3,p2,p1;
  double a,b,s,*x,*w;
  
  a = r_plow(X,p);
  b = r_phigh(X,p);
  
  x = (double *)malloc((unsigned) (n+1)*sizeof(double));
  if (!x) {
    fprintf(stderr,"memory allocation failure...");
    exit(1);
  }
  w = (double *)malloc((unsigned) (n+1)*sizeof(double));
  if (!w) {
    fprintf(stderr,"memory allocation failure...");
    exit(1);
  }
  
  m=(n+1)/2;
  xm=0.5*(b+a);
  xl=0.5*(b-a);
  for (i=1;i <= m;i++)  {
    z=cos(PI*(i-0.25)/(n+0.5));
    do {
      p1=1.0;
      p2=0.0;
      for (j=1;j <= n;j++) {
	p3=p2;
	p2=p1;
	p1=((2.0*j-1.0)*z*p2-(j-1.0)*p3)/j;
      }
      pp=n*(z*p1-p2)/(z*z-1.0);
      z1=z;
      z=z1-p1/pp;
    } while (fabs(z-z1) > LIM);
    x[i]=xm-xl*z;
    x[n+1-i]=xm+xl*z;
    w[i]=2.0*xl/((1.0-z*z)*pp*pp);
    w[n+1-i]=w[i];
  }
  
  s=0;
  for (j=1;j <= n;j++) {
    w_pval(X,p,x[j]);
    s += w[j]*FUNC(X);
  }
  free((double*) (x));
  free((double*) (w));
  
  return s;
}

/******* exemplo ********

double fij_gauss(int i,int j,double delta,double p2) 
{
    double integral0(),GaussLegendreInt(),af,fij();
    int np=15;
    void w_f_ptr(),clear_struct();
    Function X;

    if(p2>0.0) {
      clear_struct(&X,6);
      w_f_ptr(&X,integral0);
      X.par[0].low_v  = (delta-6*p2);
      X.par[0].high_v  = (delta+6*p2);
      X.par[1].val  = delta;
      X.par[2].val  = p2;
      X.par[3].val  = (double) i;
      X.par[4].val  = (double) j;
      X.par[6-1].val  = (double) np;

      af = GaussLegendreInt(&X,0,np);
    }
    else af=fij(i,j,delta);
    return af;
}
double integral0(Function *X)
{
   double r_pval(),r_plow()af;
   double p1,p2,p3,p4,p5;

   p1 = r_pval(X,0);
   p2 = r_pval(X,1);
   p3 = r_pval(X,2);
   p4 = r_pval(X,3);
   p5 = r_pval(X,4);
    {

       double theta, c, delta, p, s, s2, s4, af;
       int i,j;
       c=p3;
       delta=p2;
       theta=p1;
       i=(int) p4;
       j=(int) p5;
       
       p=exp(-pow(theta-delta,2.0)/(2*c*c))/(c*sqrt(2*pi));
       
       s  = sin(theta);
       s2 = s*s;
       s4 = s2*s2;
       
       if(j==0 && i==0)      af = 8-24*s2+18*s4;
       else if(j==1 && i==0) af = 144*(s2 - s4);
       else if(j==2 && i==0) af = 9*s4;
       
       else if(j==0 && i==1) af = 2*(s2-s4);
       else if(j==1 && i==1) af = 8-20*s2+16*s4;
       else if(j==2 && i==1) af = 2*s2-s4;
       
       else if(j==0 && i==2)      af = 2*s4;
       else if(j==1 && i==2) af = 16*(2*s2 - s4);
       else   af = 8-8*s2+s4;
       
       af=af/8.0;
       
       return af*p;


    }
}

*******/
