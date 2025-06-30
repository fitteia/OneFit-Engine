#include <math.h>
#include <stdio.h>
#define pi 3.1415927
#include "SDN11_2_DllDp2.h"
#include "sdvilfjwn11_2.h"

double SDN11_2_DllDp2(double f, double d, double n, double tauD, double delta)
{
  double 	w,af,tau;

  w   = 2*pi*f;

  af  = sdvilfjwN11_2(0.5,d,n,tauD,w,delta);
  
  return af;
}
