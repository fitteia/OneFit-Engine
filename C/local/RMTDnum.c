#include <math.h>
#include <stdio.h>
#include "globals.h"
#include "struct.h"

double RMTDnum(double f, double p, double Armtd, double fcm,double fcM)
{
	double	x;
	double	sqgaus(),sqromo(),smidpnt(),jrmtd(),A,j1,j2,af;
	void	w_f_ptr(),clear_struct();

	af =pow(1.0/f,p)*jrmtd(f,fcM,fcm,p) + 4*pow(0.5/f,p)*jrmtd(2*f,fcM,fcm,p);
/*	printf("af=%le\n",af); */

	return Armtd*af;

}
  /** XGFITN ADITIONAL CODE START **/
/* auxliar code for ./tmp/tmp-difraction.c */

double jrmtd_int(Function *X)
{
	double x,xM,xm,p,af;
	double r_pval(),r_plow();

	x    = r_pval(X,0);
	p    = r_pval(X,1);

/*	printf("%g\n",Vi);*/
	af = pow(x,3.0-2.0*p)/(1+pow(x,4.0));
/*printf("af=%e\n",af);*/
	return af;
}

double jrmtd_int1(Function *X)
{
	double x,xM,xm,p,af;
	double r_pval(),r_plow();

	x    = r_pval(X,0);
	p    = r_pval(X,1);

/*	printf("%g\n",Vi);*/
	af = pow(x,2.0*p-1.0)/(1+pow(x,4.0));
/*printf("af=%e\n",af);*/
	return af;
}


double jrmtd(double f,double fcM,double fcm,double p)
{
	double	sqgaus(),sqromo(),smidpnt(),jrmtd_int(),jrmtd_int1(),A,j1,j2,af;
	void	w_f_ptr(),clear_struct();
	Function X;

	clear_struct(&X,2);

	if(f<fcm){
		X.par[0].low_v  = sqrt(f/fcM);
		X.par[0].high_v = sqrt(f/fcm); 
		X.par[1].val    = p;
		w_f_ptr(&X,jrmtd_int1);

		j2 = sqgaus(&X,0); 	
/*		j1 = sqromo(&X,smidpnt,0); */

	}	
	else if(f>fcm && f<fcM) {
		X.par[0].low_v  = sqrt(fcm/f);
		X.par[0].high_v = 1.0; 
		X.par[1].val    = p;
		w_f_ptr(&X,jrmtd_int);

		j2 = sqgaus(&X,0); 
/*		j1 = sqromo(&X,smidpnt,0); */

		X.par[0].low_v  = sqrt(f/fcM);
		X.par[0].high_v = 1.0; 
		X.par[1].val    = p;
		w_f_ptr(&X,jrmtd_int1);


		j2 += sqgaus(&X,0); 
/*		j1 += sqromo(&X,smidpnt,0); */
	}
	else {
		X.par[0].low_v  = sqrt(fcm/f);
		X.par[0].high_v = sqrt(fcM/f); 
		X.par[1].val    = p;
		w_f_ptr(&X,jrmtd_int);

		j2 = sqgaus(&X,0); 	
/*		j1 = sqromo(&X,smidpnt,0); */

	}
/*	printf("j1=%le j2=%le\n",j1,j2); */
/*	printf("j2=%le\n",j2); */
	return j2;
}




























































































 /** XGFITN ADITIONAL CODE END **/
