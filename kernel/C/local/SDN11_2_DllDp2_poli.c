#include <math.h>
#include <stdio.h>
#define pi 3.1415927
double SDN11_2_DllDp2_poli(double f, double d, double n, double tauD)
{
  double 	w,af,tau;
  double	sdvilfjwN11_2_poli();


  w   = 2*pi*f;

  af  = sdvilfjwN11_2_poli(0.5,d,n,tauD,w);
  
  return af;
}
