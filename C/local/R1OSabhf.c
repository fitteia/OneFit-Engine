
#include <math.h>
#include <stdio.h>
#include "lor.h"
#include "R1OSabhf.h"

#define pi 3.1415926

#define gammae 1.76085963023e11
#define gamma 2.6752212881e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

/*****
*      f: frequency; Unite: Hz
*      C: mili molar concentration of particles m; Unit: mmol/L
*      S: electronic spin; Unit: none
*      a: distance of closet approach betwee outer-shere molecules and m paticles; Unit: m 
*      D: self-diffusion coefficient; Unit; m^2/s
*     tv: correlation time of the zero field splitting fluctuations; Unit; s
* Delta2: Means sqaure fluctuation sof the zero field spliting of the m species; Unit: s^{-2}
*****/

double R1OSabhf(double f, double C, double S, double a, double D, double tv, double Delta2)
{
  double x,wI,wS, Delta,af,tD,it1s,it2s;
  
   wI     = 2*pi*f;
   wS    = wI*gammae/gamma;
  
   it1s  = 1.0/25.0*Delta2*(4*S*(S+1)-3)*(lor(tv,wS)+4.0*lor(tv,2*wS));
   it2s  = 1.0/50.0*Delta2*(4*S*(S+1)-3)*(3*tv+5.0*lor(tv,wS)+2.0*lor(tv,2*wS));

   tD    = a*a/D;

   Delta = hbar*gammae*gamma*1e-7;

   af    = 32*pi/405.0*Delta*Delta*S*(S+1)*NA*C/(a*D)*(jhf(tD*it2s,tD*(fabs(wS-wI)))+3*jhf(tD*it1s,tD*wI)+6*jhf(tD*it2s,tD*(wS+wI)));

   return af;
}

double jhf(double re, double im)
{
  double A,B,u,v,af;

  A  = sqrt(sqrt(re*re+im*im));
  B  = (re>0.0) ? atan(im/re)/2.0 : pi/4.0;

  u  = A*A*A*cos(3*B)+9*A*cos(B)+4*re+9.0;
  v  = A*A*A*sin(3*B)+9*A*sin(B)+4*im;

  af = 9.0/4.0*(A*v*sin(B)+A*u*cos(B)+4*u)/(u*u+v*v);
  
  return af;
}
