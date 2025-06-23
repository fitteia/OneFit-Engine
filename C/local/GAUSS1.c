#include <math.h>
#include <stdio.h>
#include "GAUSS1.h"

#define pi 3.14159265

double GAUSS1(double f, double a, double b, double f0)
{
  double	af;

  af= a/sqrt(pi)*exp(-pow((f-f0)/b,2.0));
return af;

}


































 /** XGFITN ADITIONAL CODE END **/
