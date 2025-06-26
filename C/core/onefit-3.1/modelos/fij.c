/******************************************************************************/
/*				FIJ.C					      */
/******************************************************************************/
#include <stdio.h>
#include <math.h>
#include "fij.h"
#include "fitutil.h"

double	fij(int i, int j, double delta)
{
	double 	af,s,s2,s4;

	s  = sin(delta);
	s2 = s*s;
	s4 = s2*s2;
	switch(i) {
		case 0:
			if(j==0)      af = 8-24*s2+18*s4;
			else if(j==1) af = 144*(s2 - s4);
			else if(j==2) af = 9*s4;
			else nrerror("fij(i,j,delta) not defined.");
			break;
		case 1:
			if(j==0)      af = 2*(s2-s4);
			else if(j==1) af = 8-20*s2+16*s4;
			else if(j==2) af = 2*s2-s4;
			else nrerror("fij(i,j,delta) not defined.");
			break;
		case 2:
			if(j==0)      af = 2*s4;
			else if(j==1) af = 16*(2*s2 - s4);
			else if(j==2) af = 8-8*s2+s4;
			else nrerror("fij(i,j,delta) not defined.");
			break;
		default:
			nrerror("fij(i,j,delta) not defined.");
			break;
	}
	return af/8.0;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	fh(int h, double delta)
{
	double	fij();
	
	return fij(h,1,delta);
}
/******************************************************************************/
/*									      */
/******************************************************************************/
