#include <math.h>
#include <stdio.h>

#define pi 3.1415927  


double BPP(double f,double a,double tauc)
{

double w,af,lor();
w = 2*pi*f;
af  = a*(lor(tauc,w)+4*lor(tauc,2*w));

return af;
}
