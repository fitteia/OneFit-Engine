#include <math.h>
#include <stdio.h>

#define pi 3.1415927

double R1Torrey(double f, double n, double d, double r, double tau)
{
  double JTorrey(),alpha,af,w;
  double Kdd=8.5444e-49;  /* 3/2*(mu0/(4*pi))^2*gamma^4*hbar^2 */

  af= Kdd*3.0/4.0*(JTorrey(f,n,d,r,tau)+4*JTorrey(2*f,n,d,r,tau));
  return af;
}

double JTorrey(double f, double n, double d, double r, double tau)
{
  /* J(0)=6J(1); J(2)=4J(1) */
  double jTorrey(),alpha,w;
  
  alpha=r*r/(12*d*d);
  w=2*pi*f;
  
  return n*tau/(d*d*d)*jTorrey(alpha,w*tau);
}
  

double jTorrey(double alpha, double wt)
{
  double af,q,up,um;

  q=wt/sqrt(4+wt*wt);
  up=0.5*sqrt(q*(1+q)/alpha);
  um=0.5*sqrt(q*(1-q)/alpha);
  af=4.0/15.0*(8*pi*alpha/(wt*wt))*(
     (up*(1+1.0/(um*um+up*up))+2)*exp(-2*up)*cos(2*um)+
     (up*(1-1.0/(um*um+up*up)))+
     (um*(1-1.0/(um*um+up*up)))*exp(-2*up)*sin(2*um)
  );
  return af;
}
