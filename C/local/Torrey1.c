#include <math.h>
#include <stdio.h>

#define pi 3.1415927  


double Torrey1(double f,double d,double r, double n,  double tauD)
{

double w,af,t1torrey();
w = 2*pi*f;
af  = t1torrey(0.5,d,n,r*r/(12.0*d*d),tauD,w,0.0);

return af;
}
