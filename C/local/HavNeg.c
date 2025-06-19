#include <math.h>
#include <stdio.h>

#define pi 3.1415927  

double JHN(double w, double t, double epsilon, double delta)
{
  double af,wt,wtd,wt2d,sind,cosd;

  wt=w*t;
  wtd=pow(wt,delta);
  wt2d=pow(wt,2*delta);
  sind=sin(delta*pi/2.0);
  cosd=cos(delta*pi/2.0);
  
  if(wt==0.0) { af=1e99; }
  else {
    af= 2.0/w*sin(epsilon*atan(wtd*sind/(1+wtd*cosd)))*pow(1+2*wtd*cosd+wt2d,-epsilon/2.0);
  }
  return af;
}

double HavNeg(double f,double A,double tau, double epsilon, double delta)
{
  double w,af;
  w = 2*pi*f;

  af=A*(JHN(w,tau,epsilon,delta)+4*JHN(2*w,tau,epsilon,delta));

  return af;
}


