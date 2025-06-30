#include <math.h>
#include <stdio.h>
#include "ROUSE1234.h"

#define pi 3.1415927  


double ROUSE1234(double f, double a1, double f0,double f1, double f2, double p1, double p2, double p3, double p4)
{
	double w,af;
	w = 2*pi*f;
	if(f>f0 && f>f1 && f>f2){
        af=a1*pow(f,p1);
	}
	else if(f>f1 && f<=f0 && f>f2){
        af=a1*pow(f0,p1-p2)*pow(f,p2);
	}
	else if(f<=f1 && f<f0 && f>f2){
        af=a1*pow(f0,p1-p2)*pow(f1,p2-p3)*pow(f,p3);
	}
	else if(f<f1 && f<f0 && f<=f2){
        af=a1*pow(f0,p1-p2)*pow(f1,p2-p3)*pow(f2,p3-p4)*pow(f,p4);
	}

	return af;
}
