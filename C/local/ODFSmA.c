#include <math.h>
#include <stdio.h>
#include "ODFSmA.h"
#include "odf.h"

#define	pi	3.141592653589793238

double ODFSmA(double f, double Alu, double fcm, double fcM, double delta)
{
  double w,wcm,wcM,af,odfSa();
  int i=1;

  wcm = 2*pi*fcm;
  wcM = 2*pi*fcM;
  w = 2*pi*f;
  

  af  = Alu*odfSa(wcm,wcM,w,delta);
  
  return af;
}
