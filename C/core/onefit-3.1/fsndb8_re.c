/******************************************************************************/
/*				FSNDB8.C				      */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "globals.h"

#define	W0	2*pi*56.0e6
#define	pi	3.141592653589793238



/******************************************************************************/
/*									      */
/******************************************************************************/
double	f1(f,par,t,mode) /*$ sdvilfSa5(mode) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	double 	w,af,tauD,d,n,delta;
	double	sdvilfSa5();

	while( t != _T[i] ) i++;
	tauD= par[9]*exp(par[10]*(1/_T[i]-1/_T[1]));
	d   = par[11];
	n   = par[11+i];
	if( mode == 0){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}
	af  = sdvilfSa5(0.5,d,n,tauD,w,delta);

	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f2(f,par,t,mode)/*$ sdvilfSa7(mode) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	double 	w,af,tauD,d,n,delta;
	double	sdvilfSa7(),t1ro7();

	while( t != _T[i] ) i++;
	tauD= par[9]*exp(par[10]*(1/_T[i]-1/_T[1]));
	d   = par[11];
	n   = par[11+i];
	if( mode == 0){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}
	af  = sdvilfSa7(0.5,d,n,tauD,w,delta);

	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f3(f,par,t,mode)/*$ sdvilfSa11(mode) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	double 	w,af,tauD,d,n,delta;
	double	sdvilfSa11(),t1ro11();

	while( t != _T[i] ) i++;
	tauD= par[9]*exp(par[10]*(1/_T[i]-1/_T[1]));
	d   = par[11];
	n   = par[11+i];
	if( mode == 0){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}
	af  = sdvilfSa11(0.5,d,n,tauD,w,delta);

	return af;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	f4(f,par,t,mode)/*$ sdvilf5(f,delta) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	double 	w,af,tauD,d,n,delta;
	double	sdvilf5(),f20();

	while( t != _T[i] ) i++;
	tauD= par[9]*exp(par[10]*(1/_T[i]-1/_T[1]));
	d   = par[11];
	n   = par[11+i];
	if( f>180.0 ){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}
	af  = sdvilf5(0.5,d,n,tauD,w,delta);

	if(f>180.0) return af;
	else return af/f20(par[13],par,t,mode);
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	f5(f,par,t,mode)/*$ sdvilfN11_2(f) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	double 	w,af,tauD,d,n,delta;
	double	sdvilfN11_2(),t1ro11();

	while( t != _T[i] ) i++;
	tauD= par[9]*exp(par[10]*(1/_T[i]-1/_T[1]));
	d   = par[11];
	n   = par[11+i];
	delta = 0.0;
	w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	af  = sdvilfN11_2(0.5,d,n,tauD,w,delta);

	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f6(f,par,t,mode)/*$ sdvilfjwN11_2(mode) $*/
int	mode;
double   f,t;
double	par[];
{
	int	i=1;
	double 	w,af,tauD,d,n,delta;
	double	sdvilfjwN11_2(),t1roN11();

	while( t != _T[i] ) i++;
	tauD= par[9]*exp(par[10]*(1/_T[i]-1/_T[1]));
	d   = par[11];
	n   = par[11+i];
	if(mode == 0) {
		delta= 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}	
	af  = sdvilfjwN11_2(0.5,d,n,tauD,w,delta);

	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f7(f,par,t,mode)/*$ rot(f) (lorentzianas) $*/
int	mode;
double   f,t;
double	par[];
{
	int	i=1;
	double 	af,tauR,w1,w,delta,B;
	double	rot();

	while( t != _T[i] ) i++;
	delta = 0.0;
	tauR = par[3]*exp(par[4]*(1/_T[i]-1/_T[1]));
	w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	af  = rot(tauR,w,delta);

	return af;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	f8(f,par,t,mode)/*$ odf_k123(mode) $*/
int	mode;
double 	f,t;
double	par[];
{
	double	Q,fb,K13,K23,K31,K21,eta12,RM,Rm,delta,af;
	double	odf_k123(),odfSN(),odfN();
	double	wcb,wcm,wcM,w;

	wcm   = 2*pi*par[5];
	wcM   = 2*pi*par[6];
	K31   = par[24];
	K21   = par[25];
	eta12 = par[26];
	RM    = par[27];
	Rm    = par[28];
	Q     = par[29];
	wcb   = 2*pi*par[30];
	
	K13   = 1.0/K31;
	K23   = K21/K31;

	if( mode == 0){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}

/*	af  = odfN(wcm,wcM,w,delta);					*/
/*	af=odfSN(w,delta,K31,K21,eta12,RM,Rm,wcM,wcm,wcb,Q);		*/

	af=odf_k123(w,delta,K13,K23,eta12,RM,Rm,wcM/K13,wcm/K13);	

	return af;

}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f9(f,par,t,mode)/*$ lor(f) $*/
int	mode;
double 	f,t;
double	par[];
{
	double	tauc,af,w,w0;
	double	lor();

	tauc= par[7];
	w0  = 2*pi*par[8];
	w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	af  = lor(tauc,w-w0);
	return af;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	f10(f,par,t,mode)/*$ odfSa(mode) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	double	wcm,wcM,w,delta,af;
	double	odfSa();

	wcm = 2*pi*par[5];
	wcM = 2*pi*par[6];

	while( t != _T[i] ) i++;
	if( mode == 0){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}
	af  = odfSa(wcm,wcM,w,delta);
	
	return af;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	f11(f,par,t,mode)/*$ rot_reor(mode) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	double 	af,tauS,tauL,S,P4,A0,A1,A2,w1,w,w0,delta;
	double	rot_reor(),rot_reor_ro();

	while( t != _T[i] ) i++;

	tauS  = par[3]*exp(par[4]*(1/_T[i]-1/_T[1]));
	tauL  = tauS/par[15];
	S     = par[16]*pow(1-_T[i]/par[17],par[18]);
	P4    = 5.0*S*S/7.0;
	A0    = par[19];
	A1    = par[20];
	A2    = par[21];

	if( mode == 0){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
	}
	af=rot_reor(tauS,tauL,w,delta,S,P4,A0,A1,A2);

	return af;
}

/******************************************************************************/
/*ROT									      */
/******************************************************************************/
double	f12(f,par,t,mode)/*$ rot_reor(f,delta) $*/
int	mode;
double 	f,t;
double	par[];
{
	double	f11(),f20(),af;

	if(f>180.0) af=f11(f,par,t,0);
	else af=f11(f,par,t,1)/f20(par[13],par,t,mode);
	return af;
}

/******************************************************************************/
/*ODFN									      */
/******************************************************************************/
double	f13(f,par,t,mode)/*$ odf_k123(f,delta) $*/
int	mode;
double 	f,t;
double	par[];
{
	double	f8(),f20(),af;

	if(f>180.0) af=f8(f,par,t,0);
	else af=f8(f,par,t,1)/f20(par[13],par,t,mode);
	return af;
}
/******************************************************************************/
/*ODFSA									      */
/******************************************************************************/
double	f14(f,par,t,mode)/*$ odfSa(f,delta) $*/
int	mode;
double 	f,t;
double	par[];
{
	double	f10(),f20(),af;

	if(f>180.0) af=f10(f,par,t,0);
	else af=f10(f,par,t,1)/f20(par[13],par,t,mode);
	return af;
}
/******************************************************************************/
/*SDN									      */
/******************************************************************************/
double	f15(f,par,t,mode)/*$ sdvilfjwN11_2(f,delta) $*/
int	mode;
double 	f,t;
double	par[];
{
	double	f6(),f20(),af;

	if(f>180.0) af=f6(f,par,t,0);
	else af=f6(f,par,t,1)/f20(par[13],par,t,mode);
	return af;
}
/******************************************************************************/
/*SDSA7									      */
/******************************************************************************/
double	f16(f,par,t,mode)/*$ sdvilfSa7(f,delta) $*/
int	mode;
double   f,t;
double	par[];
{
	double	f2(),f20(),af;

	if(f>180.0) af=f2(f,par,t,0);
	else af=f2(f,par,t,1)/f20(par[13],par,t,mode);
	return af;
}
/******************************************************************************/
/*LOR									      */
/******************************************************************************/
double	f17(f,par,t,mode)/*$ lor(f,delta) $*/
int	mode;
double   f,t;
double	par[];
{
	double	f9();

	if(f>180.0) return f9(f,par,t,mode);
	else return 1e-15;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f18(f,par,t,mode)/*$ dimer(f,delta) $*/
int	mode;
double 	f,t;
double	par[];
{
	double	a0,a1,a2,tau,w,delta,af;
	double	dimer(),f20();

	a1  = par[31];
	a2  = par[32];
	tau = par[33];
	a0  = 8*a1-a2/2.0;  /*  a dependencia em sin^4(delta e nula  */

	if( f > 180.0){
		delta = 0.0;
		w   = 2*pi*sqrt(f*f + par[2]*par[2]);
		af = dimer(a0,a1,a2,tau,w,delta);
	} else {
		delta = pi*f/180.0;
		w   = 2*pi*sqrt(par[13]*par[13] + par[2]*par[2]);
		af = dimer(a0,a1,a2,tau,w,delta)/f20(par[13],par,t,mode);
	}
	
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f19(f,par,t,mode)/*$ T1_rot_vold(f,delta) $*/
int	mode;
double 	f,t;
double	par[];
{
	int	i=1;
	float	delta,freq,Szz,Tau_S,Tau_L,K1,p,f0,Afact0,Afact1,Afact2;
	double	af;
	double	T1_rot_vold(),f20();

	while( t != _T[i] ) i++;

	Tau_S  = (float) par[3];
	Tau_L  = (float) Tau_S/par[15];
	Szz    = (float) par[16]*pow(1-_T[i]/par[17],par[18]);
	Afact0 = (float) par[19]*1e-3;
	Afact1 = (float) par[20]*1e-3;
	Afact2 = (float) par[21]*1e-3;
/*	K1     = (float) par[22];       */
	K1     = (float) par[22]/Tau_L;
	p      = (float) par[23];

	if(f>180.0) {
		delta= 0.0;
		freq = (float) sqrt(f*f+par[2]*par[2]); 
	}
	else {
		delta = (float) f; 
		f0    = (float) par[13];
		freq  = (float) sqrt(f0*f0+par[2]*par[2]);
	}	
	af = T1_rot_vold(freq,delta,Szz,Tau_S,Tau_L,K1,p,Afact0,Afact1,Afact2);
	
	if(f>180.0) return af;
	else return af/f20(par[13],par,t,mode);
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	f20(f,par,t,mode)/*$ sum(i=1,num. meca.) (1/T1(x))i $*/
int	mode;
double 	f,t;
double	par[];
{
	int	j;
	double 	sum,*afunc;
	double 	*dvector();
	void 	funcsn(),free_dvector();

	afunc= dvector(1,Ma);
	funcsn(f,afunc,Ma,par,t,mode);
	for (sum=0.0,j=1;j<=Ma;j++) sum += AA[j][1]*afunc[j];
	free_dvector(afunc,1,Ma);
	return sum;

}
/******************************************************************************/
/*									      */
/******************************************************************************/
/*$ Funcoes de ajuste do DB8CN Sym para dados angulares reduzidos $*/
