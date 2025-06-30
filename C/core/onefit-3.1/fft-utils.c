/******************************************************************************/
/*									      */
/*				     FFT				      */
/*									      */
/******************************************************************************/
#include	<math.h>
#include	<stdio.h>
#include	"struct.h"
#include "fft-utils.h"

#define		SWAP(a,b)	tempr=(a);(a)=(b);(b)=tempr

/******************************************************************************/
/*									      */
/******************************************************************************/
void	dfour1(double data[], int nn, int isign)
// double	data[];
// int	nn,isign;
/*
Replaces data by its discrete Fourier transform, if isign is input as 1; or 
replaces data nn times its inverse discrete Fourier transform, if isign is input
as -1. data is a complex array of length nn, input as a real array data[1..2*nn]
nn MUST be an integer power of 2 (this is not checked for!).
*/
{
	int	n,mmax,m,j,istep,i;
	double	wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	n=nn << 1;
	j=1;
	for(i=1;i<n;i+=2) {
		if(j>i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while(m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while(n > mmax) {
		istep = 2*mmax;
		theta = 6.28318530717959/(isign*mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr  = 1.0;
		wi  = 0.0;
		for(m=1;m<mmax;m+=2) {
			for(i=m;i<=n;i+=istep) {
				j = i+mmax;
				tempr = wr*data[j]-wi*data[j+1];
				tempi = wr*data[j+1]+wi*data[j];
				data[j] = data[i]-tempr;
				data[j+1] = data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr = (wtemp=wr)*wpr-wi*wpi+wr;
			wi = wi*wpr+wtemp*wpi+wi;
		}
		mmax = istep;
	}
}
/******************************************************************************/
/*									      */
/******************************************************************************/
void dtwofft(double data1[], double data2[], double fft1[], double fft2[], int n)
// double	data1[],data2[],fft1[],fft2[];
// int	n;
/*
Given two real input arrays data1[1..n] and data2[1..n], this routine calls 
four1 and returns two complex output arrays, fft1, fft2. each of complex length
n (i.e. real dimensions [1..2n], which contain the discrete fourier transforms 
of the respective datas. n MUST be an integer power of 2.
*/
{
	int	nn3,nn2,jj,j;
	double	rep,rem,aip,aim;

	nn3 = 1+(nn2=2+n+n);
	for(j=1,jj=2;j<=n;j++,jj += 2) {
		fft1[jj-1]=data1[j];
		fft1[jj]=data2[j];
	}
	dfour1(fft1,n,1);
	fft2[1]=fft1[2];
	fft1[2]=fft2[2]=0.0;
	for(j=3;j<=n+1;j+=2) {
		rep = 0.5*(fft1[j]+fft1[nn2-j]);
		rem = 0.5*(fft1[j]-fft1[nn2-j]);
		aip = 0.5*(fft1[j+1]+fft1[nn3-j]);
		aim = 0.5*(fft1[j+1]-fft1[nn3-j]);
		fft1[j]    = rep;
		fft1[j+1]  = aim;
		fft1[nn2-j]= rep;
		fft1[nn3-j]= -aim;
		fft2[j]	   = aip;
		fft2[j+1]  = -rem;
		fft2[nn2-j]= aip;
		fft2[nn3-j]= rem;
	}
}
/******************************************************************************/
/*									      */
/******************************************************************************/
void drealfft(double data[], int n, int isign)
// double	data[];
// int	n,isign;
/*
Calculates the Fourier Transform of a set of 2n real-valued data points.
Replaces this data (which is stored in array data[1..2n] by the positive 
frequency half of its complex Fourier Transform. The real-valued first and last
components of the complex transforms are returned as elements data[1] and 
data[2] respectively. n MUST be a power of 2. This routine also calculates
the inverse transform of a complex data array if it is the transform of real 
data.(Result in this case must be multiplied by 1/n.)
*/
{
	int	i,i1,i2,i3,i4,n2p3;
	double	c1=0.5,c2,h1r,h1i,h2r,h2i;
	double	wr,wi,wpr,wpi,wtemp,theta;

	theta = 3.141592653589793/(double) n;
	if(isign == 1) {
		c2 = -0.5;
		dfour1(data,n,1);

	} else {
		c2 = 0.5;
		theta = -theta;
	}
	wtemp = sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi = sin(theta);
	wr = 1.0+wpr;
	wi = wpi;
	n2p3 = 2*n+3;
	for(i=2;i<=n/2;i++) {
		i4  =  1+(i3=n2p3-(i2=1+(i1=i+i-1)));
		h1r =  c1*(data[i1]+data[i3]);
		h1i =  c1*(data[i2]-data[i4]);
		h2r = -c2*(data[i2]+data[i4]);
		h2i =  c2*(data[i1]-data[i3]);
		data[i1] =  h1r+wr*h2r-wi*h2i;
		data[i2] =  h1i+wr*h2i+wi*h2r;
		data[i3] =  h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr = (wtemp=wr)*wpr-wi*wpi+wr;
		wi = wi*wpr+wtemp*wpi+wi;
	}
	if(isign == 1) {
		data[1] = (h1r=data[1])+data[2];
		data[2] = h1r - data[2];
	} else {
		data[1]=c1*((h1r=data[1])+data[2]);
		data[2]=c1*(h1r-data[2]);
		dfour1(data,n,-1);
	}
}
/******************************************************************************/
/*									      */
/******************************************************************************/
void expri(double data[], double re[], double im[], int n)
// double	data[],re[],im[];
// int	n;
/*
Extract Real and Imaginary data out of a positive real-valued Complex data 
vector.
*/
{
	int	i,j;

	re[0]=data[1];
	re[n]=data[2];
	im[1]=im[n]=0.0;
	for(i=3,j=1;i<2*n;i += 2,j++) {
		re[j]=data[i];
		im[j]=data[i+1];
/*		printf("%d %d\n",i,j);					      */
	}
}
/******************************************************************************/
/*									      */
/******************************************************************************/
void exri(double data[], double re[], double im[], int n)
// double	data[],re[],im[];
// int	n;
/*
Extract Real and Imaginary data out of a real-valued Complex data vector
*/
{
	int	j;
	
	re[n]=data[n+1];
	im[n]=data[n+1];
	for(j=0;j<n/2;j++) {
		re[j]=data[n+1+2*j];
		re[n/2+j]=data[2*j+1];
		im[j]=data[n+2*j+2];
		im[n/2+j]=data[2*j+2];
	}
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void area_fft(double Re_data[], double Im_data[], int npoints, double step, double *a_Re, double *a_Im)
// double	Re_data[],Im_data[];
// int	npoints;
// double	step;
// double	*a_Re;
// double	*a_Im;
{
	int	i;

	*a_Re = *a_Im = 0.;
	for(i = 0; i < npoints; i++) {
		*a_Re += Re_data[i];
		*a_Im += Im_data[i];
	}
	*a_Re *= step;
	*a_Im *= step;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double pow_fft(double Re_data[], double Im_data[], int npoints, double step)
// double	Re_data[],Im_data[];
// int	npoints;
// double	step;
{
	int	i;
	double	af=0.;

	for(i = 0; i < npoints; i++) {
		af += Re_data[i]*Re_data[i]+Im_data[i]*Im_data[i];
	}
	return( step*af );
}
/******************************************************************************/
/*									      */
/******************************************************************************/
