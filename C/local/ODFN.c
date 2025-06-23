#include <math.h>
#include <stdio.h>
#include "ODFN.h"
#include "odf.h"

#define pi 3.1415927  

double ODFN(double f, double Aodf, double fcm, double fcM, double delta)
{
  double	wcm,wcM,w,af;

  wcm   = 2*pi*fcm;
  wcM   = 2*pi*fcM;
  w   = 2*pi*f;

  af= Aodf*odfN(wcm,wcM,w,delta);	
  
  return af;

}


































 /** XGFITN ADITIONAL CODE END **/
