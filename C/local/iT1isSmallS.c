#include <math.h>
#include <stdio.h>

#define pi 3.1415926

#define gammae 1.76e11
#define gamma 2.67513e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

double iT1isSmallS(double f, double T, double tmg, double tmH, double tv, double ZFS, double r,double Sp,double S)
{
  double x,w, Delta,af,Sc,ws,td1,td2,td1g,td2g,it1s,it2s;
  double __Jin();

   w = 2*pi*f;
   ws=w*gammae/gamma;
  
   it1s=1.0/25.0*ZFS*ZFS*(4*Sp*(Sp+1)-3)*tv*(1.0/(1+ws*ws*tv*tv)+4.0/(1+4*ws*ws*tv*tv));
   it2s=1.0/50.0*ZFS*ZFS*(4*Sp*(Sp+1)-3)*tv*(3+5.0/(1+ws*ws*tv*tv)+2.0/(1+4*ws*ws*tv*tv));
   td1=1.0/(1.0/tmH+it1s);
   td2=1.0/(1.0/tmH+it2s);
   td1g=1.0/(1.0/tmg+it1s);
   td2g=1.0/(1.0/tmg+it2s);

   Delta= hbar*gammae*gamma/pow(r,3.0)*1e-7;

   af = 2./15.*Delta*Delta*Sp*(Sp+1)*(3*__Jin(w,td1,td1g,S) + 7*__Jin(w*gammae/gamma,td2,td2g,S));
//printf("%lf %lf %lf\n",Sc,Delta,af);

   return af;
}

double __Jin(double w, double td, double tdg, double S)
{
    double af;

   af=S*S*tdg/(1+w*w*tdg*tdg)+(1-S*S)*td/(1+w*w*td*td);
   return af;
}
