#include <math.h>
#include <stdio.h>
#include "iT1osSmallS.h"
#include "iT1ISpara.h"

#define pi 3.1415926

#define gammae 1.76e11
#define gamma 2.67513e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

double iT1osSmallS(double f, double T, double D, double tv,double ZFS, double r,double Sp)
{
  double x,w, af,Sc,Delta, td,it1s,it2s,ws;

   w = 2*pi*f;
   ws=w*gammae/gamma;
  
   td=r*r/D;
   Delta= hbar*gammae*gamma*1e-7;

   it1s=1.0/25.0*ZFS*ZFS*(4*Sp*(Sp+1)-3)*tv*(1.0/(1+ws*ws*tv*tv)+4.0/(1+4*ws*ws*tv*tv));
   it2s=1.0/50.0*ZFS*ZFS*(4*Sp*(Sp+1)-3)*tv*(3+5.0/(1+ws*ws*tv*tv)+2.0/(1+4*ws*ws*tv*tv));

   af = 32*NA*pi/405.0*Delta*Delta/(r*D)*Sp*(Sp+1)*(3*J0ISpara(w,td,1.0/it1s) + 7*J0ISpara(w*gammae/gamma,td,1.0/it2s));

//printf("%lf %lf %lf\n",Sc,Delta,af);

   return af;
}
