#include <math.h>
#include <stdio.h>

#define pi 3.1415926

#define gammae 1.76e11
#define gamma 2.67513e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

double iT1ISpara(double f, double T, double td, double ts,double M, double r,double S)
{
   double J0ISpara(), J0ISpara_infts(), x,w, af,Sc,c;

   w = 2*pi*f;
   x=S*hbar*gammae*w/(gamma*k*T);


  c= (16.0*pi/135000.0)*pow(hbar*gammae*gamma,2.0)*NA*M/pow(r,3.0)*1e-14;

  Sc=S*((S+0.5)/(S*tanh((S+0.5)*x/S))-1.0/(2*S*tanh(x/(2*S))));

   af = 6*c*td*(Sc*Sc*J0ISpara_infts(w,td) + ( S*(S+1)-Sc/tanh(x/(2*S)) -Sc*Sc)*J0ISpara(w,td,ts));
   return af;
}


double J0ISpara(double w, double td, double ts)
{
   double J1;

   J1=(9/4.0)*(
36*ts*ts+td*td*sqrt(1+ts*ts*w*w)+td*ts*(16+9*sqrt(1+ts*ts*w*w))+ts*sqrt(sqrt((td*td*(pow(ts,-2.0)+w*w))))*((4*td+45*ts)*cos(0.5*atan(ts*w))+4*td*ts*(sqrt(pow(ts,-2.0)+w*w)*cos(1.5*atan(ts*w))+w*sin(0.5*atan(ts*w))))
)/
(81*ts*ts+9*td*ts*(8+9*sqrt(1+ts*ts*w*w))+2*td*td*(8+8*ts*ts*w*w+9*sqrt(1+ts*ts*w*w))+td*td*td*(sqrt(pow(ts,-2.0)+w*w)+ts*w*w*sqrt(1+ts*ts*w*w)+2*ts*sqrt(sqrt((td*td*(pow(ts,-2.0)+w*w))))*(9*(4*td+9*ts)*cos(0.5*atan(ts*w))+td*((4*td+9*ts)*sqrt(pow(ts,-2.0)+w*w)*cos(1.5*atan(ts*w))+4*ts*w*(9*sin(0.5*atan(ts*w))+td*sqrt(pow(ts,-2.0)+w*w))*sin(1.5*atan(ts*w))))));

   return J1;
}

double J0ISpara_infts(double w, double td)
{
   double J2;

   J2= 10.125*(8+2*td*w+5*sqrt(2*td*w))/(81*(1+sqrt(2*td*w))+td*w*(27*(3+sqrt(2*td*w))+td*w*(16+td*w+4*sqrt(td*w))));

  return J2;
}


