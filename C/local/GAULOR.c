#include <math.h>
#include <stdio.h>
#include "GAULOR.h"
#include "LORTZN.h"
#include "GAUSS1.h"

double GAULOR(double f, double a, double b, double f0, double t)
{
  if(t>0.5){ return LORTZN(f,a,b,f0); }
  else { return  GAUSS1(f,a,b,f0); }
}


































 /** XGFITN ADITIONAL CODE END **/
