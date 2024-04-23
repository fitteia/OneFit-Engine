#include <math.h>
#include <stdio.h>

double GLmix(double f, double a, double b, double f0, double t)
{
  double	GAUSS1(),LORTZN();

  return t*GAUSS1(f,a,b,f0)+(1-t)*LORTZN(f,a,b,f0);
}


































 /** XGFITN ADITIONAL CODE END **/
