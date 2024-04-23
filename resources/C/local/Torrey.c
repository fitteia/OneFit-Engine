#include <math.h>
#include <stdio.h>

#define pi 3.1415927  


double Torrey(double f,double d,double n,  double tauD)
{

double w,af,t1torrey();
w = 2*pi*f;
af  = t1torrey(0.5,d,n,0.083333,tauD,w,0.0);

return af;
}
