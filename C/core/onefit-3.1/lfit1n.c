#include <stdio.h>
#include "globals.h"
#include "lfit1n.h"

/******************************************************************************/
/*				LFIT1N.C				      */
/******************************************************************************/
static double sqrarg;
#define SQR(a) (sqrarg=(a),sqrarg*sqrarg)

void lfit1n( double x[], double y[], double sig[], int ndata, double a[], int ma, int lista[], 
			 int mfit, double *chisq,
			 void (*func_ptr)(double x, double *afunc, int ma, double *par, double t, int mode), 
			 double par[], double t, int mode)
// int ndata,ma,lista[],mfit,mode;
// double x[],y[],sig[],a[],*chisq,t,par[];
//void (*func_ptr)();	/* ANSI: void (*func_ptr)(float,float *,int); */
{
	int	j,i;
	double 	sum,*afunc;

	afunc= dvector(0,ma);
	*chisq=0.0;
	for (i=1;i<=ndata;i++) {
	  _X=x[i];
	  _Y=y[i];
	  _E=sig[i];
	  (*func_ptr)(x[i],afunc,ma,par,t,mode);
		/*		for (sum=0.0,j=1;j<=ma;j++) printf("a[%d]=%f\n",j,a[j]);*/
		for (sum=0.0,j=1;j<=ma;j++) sum += a[j]*afunc[j];
		*chisq += SQR((y[i]-sum)/sig[i]);
/*		printf("x=%g y=%g f=%g chisq=%g\n",x[i],y[i],sum,*chisq);*/
	}
	free_dvector(afunc,0,ma);
}
/******************************************************************************/
/*									      */
/******************************************************************************/
#undef SQR
/******************************************************************************/
/*									      */
/******************************************************************************/
