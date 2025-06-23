#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#include "ECDpoli.h"
#include "integra.h"

#define pi 3.141592653589793238


double ECDpoli(double f, double Aecd, double fcm, double fcM, double R, double n)
{

        double  af,wcm,wcM,w;

        wcm = 2*pi*fcm;
        wcM = 2*pi*fcM;

        w   = 2*pi*f;
        af = Aecd*J1ecdpoli(w,R,wcm,wcM,n);
	/*	printf("%lg %lg\n", 1.126e-71*T*A0*1e60/pow(d*1e-10,1.0)*25/wcM,af);*/

      return af;
}

double	J1ecdpoli( double w, double R, double wcm, double wcM, double n)
{
  double j1,j2,af;
  j1 = j1ecd(R,w,wcm,wcM,n);
  j2 = j1ecd(R,2.*w,wcm,wcM,n);
  af=(2./5.*(j1) + 8./5.*(j2));
  /*   printf("f=%lg j1=%lg j2=%lg af=%lg ",w/(2.*pi),j1,j2,af);*/
  
  return af;
}


double jecd_int(Function *X)
{
  double u,v,R,w,wcm,wcM,af,u4;
	
	u    = r_pval(X,0);
	v    = r_pval(X,1);
	R    = r_pval(X,2);
	w    = r_pval(X,3);
	wcm    = r_pval(X,4);
	wcM    = r_pval(X,5);
        u4   =u*u*u*u;

	af = R*pow(wcM,5./2.)*v*u4/(wcM*wcM*pow(R*v*v+u4,2.0)+w*w*u4)-R*pow(wcm,5./2.)*v*u4/(wcm*wcm*pow(R*v*v+u4,2.0)+w*w*u4);
	/*printf("integrand=%e\nx=%lg\nw=%lg\ntau0=%lg",af,x,w,tau0);*/
	return af;
}

double jecd_int1(Function *X)
{
  	double u,R,wrm,wrM,af,n;
	int np;
     
	n    = r_pval(X,6);
	np = (int) n;
	w_f_ptr(X,jecd_int);
	af = sqgausn(X,0,np); 	
	w_f_ptr(X,jecd_int1);

	/*printf("integrand=%e\nx=%lg\nw=%lg\ntau0=%lg",af,x,w,tau0);*/
	return af;
}

double j1ecd(double R,double w,double wcm,double wcM, double n)
{
  	double A,j1,j2,af;
	int np;
	Function X;

	clear_struct(&X,7);

	X.par[0].low_v  = 1e-6;
	X.par[0].high_v = 1.0; 
	X.par[1].low_v  = 1e-6;
	X.par[1].high_v = 1.0; 
	X.par[2].val    = R;
	X.par[3].val    = w;
	X.par[4].val    = wcm;
	X.par[5].val    = wcM;
	X.par[6].val    = n;
	w_f_ptr(&X,jecd_int1);
	np = (int) n;
	j1 = sqgausn(&X,1,np); 	
	/*	j1 = sqromo(&X,smidpnt,0); 	*/

/*	printf("j1=%le j2=%le\n",j1,j2); */
/*	printf("j1=%le\n",j1); */
	return j1;
}

double jecd_int2(Function *X)
{
  double u,v,R,wc,w,af;
	
	u    = r_pval(X,0);
	R    = r_pval(X,1);
	wc   = r_pval(X,2);
	w    = r_pval(X,3);

	af = u*u*(atan(R*wc/(w*u*u)+u*u*wc/w)-atan(u*u*wc/w));
	/*	printf("integrand=%lg f=%lg\n",af,w/(2*pi));*/
	return af;
}


double j1ecd1(double w,double R,double wc,double n)
{
  	double A,j1,j2,af;
	int np;
	Function X;

	clear_struct(&X,4);

	X.par[0].low_v  = 1e-6;
	X.par[0].high_v = 1.0; 
	X.par[1].val    = R;
	X.par[2].val    = wc;
	X.par[3].val    = w;
	w_f_ptr(&X,jecd_int2);
	np = (int) n;
	j1 = sqgausn(&X,0,np); 	
	/*	j1 = sqromo(&X,smidpnt,0); 	*/

/*	printf("j1=%le j2=%le\n",j1,j2); */
/*	printf("f=%lg j1=%le\n",w/(2.*pi),wc/w*j1); */
	return wc/w*j1;
}



















 /** XGFITN ADITIONAL CODE END **/
