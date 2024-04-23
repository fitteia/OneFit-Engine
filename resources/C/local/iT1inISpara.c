#include <math.h>
#include <stdio.h>

#define pi 3.1415926

#define gammae 1.76e11
#define gamma 2.67513e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

double iT1inISpara(double f, double T, double td, double ts, double r,double S)
{
  double x,w, af,Sc,Delta,Jintd,Jintau,tau;

   w = 2*pi*f;
   x=S*hbar*gammae*w/(gamma*k*T);

   tau=1.0/(1.0/td+1.0/ts);
   Jintd=1.0/(1+w*w*td*td);
   Jintau=1.0/(1+w*w*tau*tau);
   Delta= hbar*gammae*gamma/pow(r,3.0)*1e-7;

   Sc=S*((S+0.5)/(S*tanh((S+0.5)*x/S))-1.0/(2*S*tanh(x/(2*S))));

   af = 6./5.*Delta*Delta*(Sc*Sc*td*Jintd + ( S*(S+1)-Sc/tanh(x/(2*S)) -Sc*Sc)*tau*Jintau);
   //      printf("af=%lg\n",af);
   return af;
}



