#include <math.h>
#include <stdio.h>
#include "globals.h"
#include "struct.h"
#include "integra.h"
#include "OPF.h"

#define pi 3.1415926

double OPF(double f, double Aopf, double f0,double fcm, double fcM,double n)
{
	double	x;
	double	j1,af;

	j1=jopf(2.0*pi*f,1.0/(2.0*pi*f0),2.0*pi*fcm,2.0*pi*fcM,n);
	af=Aopf*j1;
	/*	printf("af=%le\n",af); */


	return af;

}
  /** XGFITN ADITIONAL CODE START **/
/* auxliar code for ./tmp/tmp-difraction.c */

double jopf_int(Function *X)
{
	double x,xM,xm,w,tau0,af;

	x    = r_pval(X,0);
	w    = r_pval(X,1);
	tau0    = r_pval(X,2);

	af = sqrt(x)/(w*w+pow(x+1.0/tau0,2.0));
	/*printf("integrand=%e\nx=%lg\nw=%lg\ntau0=%lg",af,x,w,tau0);*/
	return af;
}

double jopf(double w,double tau0,double wcm,double wcM,double n)
{
	double	A,j1,j2,af;
	int np;
	Function X;

	clear_struct(&X,3);

	X.par[0].low_v  = wcm;
	X.par[0].high_v = wcM; 
	X.par[1].val    = w;
	X.par[2].val    = tau0;
	w_f_ptr(&X,jopf_int);
	np = (int) n;
	j1 = sqgausn(&X,0,np); 	
	/*	j1 = sqromo(&X,smidpnt,0); 	*/

/*	printf("j1=%le j2=%le\n",j1,j2); */
/*	printf("j1=%le\n",j1); */
	return j1;
}




























































































 /** XGFITN ADITIONAL CODE END **/
