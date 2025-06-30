#include <math.h>
#include <stdio.h>
#include "GAUSS.h"

double GAUSS(double f, double a, double b, double f0, double Df)
{
  double	af;

  af= a*(exp(-pow((f-Df/2.0+f0)/b,2.0))+exp(-pow((f+Df/2.0+f0)/b,2.0)));
return af;

}


































 /** XGFITN ADITIONAL CODE END **/
