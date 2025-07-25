/******************************************************************************/
/*				SDVILFSa11.C				      */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "sdvilfsa11.h"
#include "fitutil.h"
#include "fij.h"

#define	C	0.569504e-12
#define ndt1	20
#define Ndt	ndt1-1

static double	jwdev[][7]={
1.000000e-03,5.566725e+01,0.000000e+00,1.579181e+00,0.000000e+00,5.531304e+00,0.000000e+00,
1.414214e-03,5.566335e+01,-2.432379e+04,1.579181e+00,-2.779459e-01,5.531304e+00,-2.820917e-01,
3.162278e-03,5.554199e+01,-1.458013e+05,1.579179e+00,-3.239437e+00,5.531303e+00,-1.265656e+00,
4.472136e-03,5.520229e+01,-1.567163e+05,1.579172e+00,-3.742417e+00,5.531300e+00,-1.584834e+00,
1.000000e-02,5.186783e+01,4.901229e+04,1.578987e+00,-2.049868e+01,5.531202e+00,-1.253597e+01,
1.414214e-02,4.942522e+01,-1.264452e+02,1.578481e+00,-2.776884e+01,5.530897e+00,-2.033361e+01,
2.514756e-02,4.376135e+01,2.282008e+04,1.573894e+00,-7.647514e+01,5.527399e+00,-7.244814e+01,
3.556403e-02,4.013169e+01,1.999484e+03,1.562963e+00,-2.049849e+01,5.517334e+00,-5.402628e+01,
2.011964e-01,1.189700e+01,7.619941e+02,1.205546e+00,8.229621e+00,4.902241e+00,1.988208e+01,
2.845347e-01,7.743500e+00,1.578379e+02,1.031903e+00,-3.082014e+00,4.585348e+00,-1.781809e+01,
4.023804e-01,4.590097e+00,9.732683e+01,7.874831e-01,5.188242e+00,4.035082e+00,2.640492e+00,
5.690518e-01,2.371204e+00,4.014942e+01,5.270426e-01,2.878278e+00,3.243938e+00,7.988411e-01,
8.047608e-01,1.205892e+00,7.137730e+00,2.995379e-01,1.711324e+00,2.211324e+00,4.718062e+00,
1.138104e+00,5.804749e-01,2.456661e+00,1.369414e-01,7.151962e-01,1.147640e+00,4.744686e+00,
1.609503e+00,2.238944e-01,8.174350e-01,4.940459e-02,1.928828e-01,4.313185e-01,1.737699e+00,
2.276181e+00,6.725165e-02,1.652083e-01,1.444143e-02,3.500440e-02,1.267577e-01,2.767009e-01,
2.449490e+00,5.107875e-02,1.577453e-01,1.094007e-02,3.489594e-02,9.604115e-02,3.130188e-01,
2.645751e+00,3.808479e-02,8.885317e-02,8.140280e-03,1.918321e-02,7.146855e-02,1.675144e-01,
3.464102e+00,1.332619e-02,5.445294e-03,2.837454e-03,1.146816e-03,2.491317e-02,1.041882e-02,
3.741657e+00,9.829142e-03,7.667270e-02,2.091736e-03,1.638159e-02,1.836561e-02,1.436605e-01
};

/******************************************************************************/
/*									      */
/******************************************************************************/
double sdvilfSa11(double I,double d,double n,double t,double w,double delta)
// double I,d,n,t,w,delta;
{
	int	j;
	double	B,D,wt,m,j1,j2,af;

	if(delta == 0.0){
		af = jw11(1,t,w) + jw11(2,t,2*w);
	}
	else {
		j1 = fij(1,0,delta)*jw11(0,t,w)+
		     fij(1,1,delta)*jw11(1,t,w)+
		     fij(1,2,delta)*jw11(2,t,w);
		j2 = fij(2,0,delta)*jw11(0,t,2*w)+
		     fij(2,1,delta)*jw11(1,t,2*w)+
		     fij(2,2,delta)*jw11(2,t,2*w);
		af = j1 + j2;		
	}
	af = 1.5*I*(I+1)*n*C*t/(d*d*d) * af;
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double jw11(int k,double t,double w)
// int	k;
// double t,w;
{
	int	j,i,p;
	double	wt,m,af,*x,*y,*y2,*xx,*yy,ypn;

	x  = dvector(1,Ndt+1);
	y  = dvector(1,Ndt+1);
	y2 = dvector(1,Ndt+1);

	wt = sqrt(w*t);
	for(j=0,i= -1;j<=Ndt;j++) {
		x[j+1] = jwdev[j][0];
		y[j+1] = jwdev[j][2*k+1];
		y2[j+1]= jwdev[j][2*k+2];
		if(wt<=x[j+1] && i== -1) i=j+1;
	}
	switch(i) {
		case 1:
			dpolint(x,y,2,wt,&af,&m);
			break;
		case -1:
			af = y[Ndt+1]*x[Ndt+1]*x[Ndt+1]/(w*w);
/*
			xx = &x[Ndt];
			yy = &y[Ndt];
			dpolint(xx-1,yy-1,2,wt,&af,&m);
*/
			break;
		default:
			dsplint(x,y,y2,Ndt+1,wt,&af);
			break;
	}
	free_dvector(x,1,Ndt+1);
	free_dvector(y,1,Ndt+1);
	free_dvector(y2,1,Ndt+1);
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	jwd11(int i,double t,double w,double delta)
// int	i;
// double	t,w,delta;
{
	int	j;
	double	af;

	for(j=0,af=0.0;j<=2;j++) af += fij(i,j,delta)*jw11(j,t,w);
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double t1ro11(double I,double d,double n,double t,double w,double w0,double delta)
// double I,d,n,t,w,w0,delta;
/* B*(J0(2*w1)+10*J1(w0)+J2(2*w0) */
{
	int	j;
	double	B,D,wt,w0t,w2t,m,J0,J1,J2,af;

	J0 = jwd11(0,t,2*w,delta);
	J1 = jwd11(1,t,w0,delta);
	J2 = jwd11(2,t,2*w0,delta);
	
/*	printf("J0=%lg J1=%lg J2=%lg\n",J0,J1,J2);	*/
	af = 3.0/8.0*I*(I+1)*n*C*t/(d*d*d) * (J0 + 10*J1 + J2);
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
