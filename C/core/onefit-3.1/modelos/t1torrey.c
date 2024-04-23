/******************************************************************************/
/*				T1TORREY.C				      */
/******************************************************************************/
#include 	<stdio.h>
#include 	<math.h>
#include        "globals.h"

#define	PI	3.141592654
#define	C	0.569504e-12

double t1torrey(I,d,n,alfa,t,w,delta)
double I,d,n,t,w,delta,alfa;
{
	double	ftorrey();
	double  af;

	af = 1.5*I*(I+1)*n*C*t/(d*d*d) * ftorrey(alfa,w,t);
	return (af);
}	
/******************************************************************************/
/*				 					      */
/******************************************************************************/
double	ftorrey(alfa,w,t)
double	alfa,w,t;
{
	double	wtau;
	double	faw();
	double	a1,a2,af;

	wtau = w*t;

	if(wtau >= 0.0001) {
		af=8./15. * PI/wtau * ( faw(alfa,wtau) + 2 * faw(alfa,2*wtau) );
	}
	else if( wtau >= 0. && wtau < 0.0001) {
		a1 = (5. * alfa + 1.) / alfa;
		a2 = ( 1. + 4 * sqrt(2) ) / (6 * pow( 2*alfa,1.5) );
		af = 8.*PI/45. * ( a1 - a2 * sqrt(wtau) );
	}
	else nrerror("Erro em ftorrey...wtau < 0.0");
	return( af );
}	

/******************************************************************************/
/*                                					      */
/******************************************************************************/
double	faw(alfa,wtau)

double	alfa,wtau;
{
	double  x,u,v,q;
	double	a1,a2,a3,af;
	
	x = sqrt( wtau/(2 * alfa) );
	q = wtau/sqrt(4. + wtau * wtau);
	u = 0.5 * sqrt( q * (1 - q) / alfa );
	v = 0.5 * sqrt( q * (1 + q) / alfa );

/*	printf("x=%lg q=%lg u=%lg v=%lg\n",x,q,u,v);			      */
	a1 = 1/(u*u + v*v);
	a2 = (v * (1 + a1) + 2) * exp(-2*v) * cos(2*u);
	a3 =  u * (1 - a1) * exp(-2*v) * sin(2*u);
	af = 2 / (x * x) * (v * (1 - a1) + a2 + a3);
	return( af );
}
/******************************************************************************/
/*                                					      */
/******************************************************************************/
