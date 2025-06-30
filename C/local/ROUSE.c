#include <math.h>
#include <stdio.h>
#include "ROUSE.h"

#define pi 3.1415927  


double ROUSE(double f,double a,double tauc)
{
	double w,af;

	w = 2*pi*f;
 	af  = -a/log(w*tauc);

	return af;
}
