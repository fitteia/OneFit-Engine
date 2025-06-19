#include <math.h>
#include <stdio.h>

extern double LORTZN(double f, double a, double b, double f0);
extern double GAUSS1(double f, double a, double b, double f0);


double GLmix(double f, double a, double b, double f0, double t)
{
  return t*GAUSS1(f,a,b,f0)+(1-t)*LORTZN(f,a,b,f0);
}


































 /** XGFITN ADITIONAL CODE END **/
