#include <math.h>
#include <stdio.h>

extern double GAUSS(double f, double a, double b, double f0, double Df);
extern double LORTZN(double f, double a, double b, double f0);

double GAULOR(double f, double a, double b, double f0, double t)
{
  if(t>0.5){ return LORTZN(f,a,b,f0); }
  else { return  GAUSS1(f,a,b,f0); }
}


































 /** XGFITN ADITIONAL CODE END **/
