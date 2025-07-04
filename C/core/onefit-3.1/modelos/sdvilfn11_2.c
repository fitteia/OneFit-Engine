/******************************************************************************/
/*				SDVILFN11_2.C				      */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "sdvilfn11_2.h"
#include "fitutil.h"

#define	C	0.569504e-12
#define ndt1	41
#define Ndt	ndt1-1

/*
                           vsdn
lsd = 11
asd = 0.5
r2sd2 = 1
DllsDT = 2

*/

static double	jwdev[][3]={
1.000000e-02,5.636184e+00,0.000000e+00,
1.732051e-02,5.636164e+00,-1.061828e+00,
2.236068e-02,5.636122e+00,-1.459508e+00,
2.645751e-02,5.636060e+00,-2.143287e+00,
3.000000e-02,5.635977e+00,-3.119701e+00,
3.162278e-02,5.635927e+00,-2.753553e+00,
5.477226e-02,5.633853e+00,-9.123123e+00,
7.071068e-02,5.629712e+00,-1.533237e+01,
8.366600e-02,5.623524e+00,-2.126898e+01,
1.000000e-01,5.610465e+00,-2.979037e+01,
1.414214e-01,5.536023e+00,-5.305549e+01,
1.732051e-01,5.420308e+00,-6.478931e+01,
2.000000e-01,5.273648e+00,-6.417315e+01,
2.236068e-01,5.107092e+00,-5.452240e+01,
2.449490e-01,4.930657e+00,-3.996103e+01,
2.645751e-01,4.752349e+00,-2.403178e+01,
2.828427e-01,4.577891e+00,-1.006862e+01,
3.000000e-01,4.410937e+00,3.990240e+00,
3.162278e-01,4.253499e+00,4.959070e+00,
3.464102e-01,3.970124e+00,4.482872e+01,
3.741657e-01,3.732270e+00,-1.546710e+01,
4.000000e-01,3.527534e+00,1.657176e+02,
4.242641e-01,3.396153e+00,-4.709346e+01,
4.472136e-01,3.270652e+00,4.458333e+00,
5.477226e-01,2.724473e+00,1.855915e+00,
6.324555e-01,2.287497e+00,6.214048e+00,
7.071068e-01,1.935583e+00,6.963398e+00,
7.745967e-01,1.650654e+00,7.562166e+00,
8.366600e-01,1.418555e+00,7.463140e+00,
8.944272e-01,1.228211e+00,7.100622e+00,
9.486833e-01,1.070988e+00,6.660414e+00,
1.000000e+00,9.401692e-01,5.840744e+00,
1.414214e+00,3.382413e-01,1.922904e+00,
1.732051e+00,1.674816e-01,8.209285e-01,
2.000000e+00,9.857606e-02,4.016767e-01,
2.236068e+00,6.454803e-02,2.254119e-01,
2.449490e+00,4.541246e-02,1.379509e-01,
2.645751e+00,3.363476e-02,8.541372e-02,
2.828427e+00,2.588940e-02,7.585608e-02,
3.000000e+00,2.053173e-02,-1.346771e-02,
3.162278e+00,1.667521e-02,2.511042e-01
};

/******************************************************************************/
/*									      */
/******************************************************************************/
double sdvilfN11_2(double I,double d,double n,double t,double w,double delta)
// double I,d,n,t,w,delta;
{
	int	j,i,p;
	double	wt,m,af,*x,*y,*y2,*xx,*yy,ypn;

	x  = dvector(1,Ndt+1);
	y  = dvector(1,Ndt+1);
	y2 = dvector(1,Ndt+1);

	wt = sqrt(w*t);
	for(j=0,i= -1;j<=Ndt;j++) {
		x[j+1] = jwdev[j][0];
		y[j+1] = jwdev[j][1];
		y2[j+1]= jwdev[j][2];
		if(wt<=x[j+1] && i== -1) i=j+1;
	}
	switch(i) {
		case 1:
			dpolint(x,y,2,wt,&af,&m);
			break;
		case -1:
			af = y[Ndt+1]*x[Ndt+1]*x[Ndt+1]/(w*w);
/*
			zRxx = &x[Ndt];
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

	af = 1.5*I*(I+1)*n*C*t/(d*d*d) * af;
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
