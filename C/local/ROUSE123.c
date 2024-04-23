#include <math.h>
#include <stdio.h>

#define pi 3.1415927  



/*
* f: frequency (w/(2pi), units: Hz
* a1: amplitude for f>f0, unit: s^(p1+1)
* f0: higher characteristic frequency: unit: Hz
* f1: lowest characteristic frequency: unit: Hz
* p0: exponent of the hight frequency regime: unit: --
* p1: exponent of the intermediate frequency regime: unit: --
* p2: exponent of the low frequency regime: unit: --
*/


double ROUSE123(double f, double a, double f0, double f1, double p0, double p1, double p2)
{
	double w,af,fM,fm;
    	w = 2*pi*f;

	if(f0>=f1){ fM=f0; fm=f1; }
	else{ fM=f1; fm=f0; }

	if(f>=fM){
        		af=a*pow(f,p0);
	}
	else if(f>fm && f<fM){
        		af=a*pow(fM,p0-p1)*pow(f,p1);
	}
	else{
        		af=a*pow(fM,p0-p1)*pow(fm,p1-p2)*pow(f,p2);
	}	

	return af;
}
