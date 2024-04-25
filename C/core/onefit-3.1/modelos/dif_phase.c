#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "struct.h"

#define CA 0.0003
#define PIO2 1.57079632679490
#define SIGN(a) ((a) > 0.0 ? 1 : -1)
/******************************************************************************/
/*				DIF_PHASE.C				      */
/******************************************************************************/
double dif_phase(H,par)
double	H,par[];
{
	Function X;
	double	a1,a2,af;
	double	x,phiM,niu,H0;
	double	dcel(),sqromo(),smidpnt(),calc_phiM(),integranda(),szero();
	void	w_f_ptr(),clear_struct();

	H0   = par[1];
	x    = par[2]-1.0;
	niu  = par[3];

	clear_struct(&X,5);

	X.par[0].low_v  = 0.0;
	X.par[0].high_v = 89.0*PIO2/90.0; 
	X.par[1].val    = H0;
	X.par[2].val    = x;
	X.par[3].val    = niu;
	X.par[4].val    = H;
	w_f_ptr(&X,calc_phiM);

	phiM = szero(&X,0,1e-6);

	clear_struct(&X,4);
	X.par[0].low_v  = 0.0;
	X.par[0].high_v = PIO2; 
	X.par[1].val    = phiM;
	X.par[2].val    = x;
	X.par[3].val    = niu;
	w_f_ptr(&X,integranda);

	a2 = sqromo(&X,smidpnt,0);
	af = (1-1.0/PIO2*H0/H*a2);

	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	integranda(X)
Function *X;
{
	double	x,phiM,qzi,niu,af,s1,s2;
	double	r_pval();

	qzi  = r_pval(X,0);
	phiM = r_pval(X,1);
	x    = r_pval(X,2);
	niu  = r_pval(X,3);
	
	s1   = sin(phiM)*sin(phiM);
	s2   = sin(qzi)*sin(qzi);
	af   = sqrt( (1+x*s1*s2)/((1-s1*s2)*(1+niu*s1*s2)) );

	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	calc_phiM(X)
Function *X;
{
	double	x,phiM,niu,alfa2,k,H0,H;
	double  af,s1,a1;
	double	r_pval(),dcel();

	phiM = r_pval(X,0);
	H0   = r_pval(X,1);
	x    = r_pval(X,2);
	niu  = r_pval(X,3);
	H    = r_pval(X,4);

	s1   = sin(phiM)*sin(phiM);
	alfa2= x*s1/(1+x*s1);
	k    = sqrt( (1+x)*s1/(1+x*s1) );

	a1   = dcel( sqrt(1-k*k),1-alfa2,1.0,1.0);
	af   = H0-PIO2*sqrt(1+x*s1)/a1*H;
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double dcel(qqc,pp,aa,bb)
double qqc,pp,aa,bb;
{
	double a,b,e,f,g,em,p,q,qc;
	void nrerror();

	if (qqc == 0.0) nrerror("Bad qqc in routine CEL");
	qc=fabs(qqc);
	a=aa;
	b=bb;
	p=pp;
	e=qc;
	em=1.0;
	if (p > 0.0) {
		p=sqrt(p);
		b /= p;
	} else {
		f=qc*qc;
		q=1.0-f;
		g=1.0-p;
		f -= p;
		q *= (b-a*p);
		p=sqrt(f/g);
		a=(a-b)/g;
		b = -q/(g*g*p)+a*p;
	}
	for (;;) {
		f=a;
		a += (b/p);
		g=e/p;
		b += (f*g);
		b += b;
		p=g+p;
		g=em;
		em += qc;
		if (fabs(g-qc) <= g*CA) break;
		qc=sqrt(e);
		qc += qc;
		e=qc*em;
	}
	return PIO2*(b+a*em)/(em*(em+p));
}
/******************************************************************************/
/*									      */
/******************************************************************************/
/*
double	szero(X,n,eps)
int	 n;
double	 eps;
Function *X;
{
	int	a,b,c,p;
	double 	x,*z,*f,*xx,x1,x2;
	double	g,_T_,N;
	double	*dvector(),bis(),hyp();
	void	free_dvector(),nrerror();

	z = dvector(-1,1);
	f = dvector(-1,1);

	xx = &(*X).par[n].val;
	x1 = (*X).par[n].low_v;
	x2 = (*X).par[n].high_v;

	z[-1] = x1; *xx = x1; f[-1]=FUNC(X);
	z[1]  = x2; *xx = x2; f[1] =FUNC(X);

	if(f[-1]*f[1] > 0.0) nrerror("E necessario ter-se F(x1)*F(x2)<=0");
	a = p = 0.0; b = 1; goto bis;

hyp:
	_T_ = N = 0.0;
	for(c= -1;c<=1;c++) {
		x = z[c];
		if(f[c]==0.0) goto out;
		if(c==0) g = (z[1]-z[-1])/f[0];
		else g = c*(z[-c]-z[0])/f[c];
		_T_ = _T_ + g * (z[c]-z[-a-b]);
		N += g;
	}	
	if(N != 0 && fabs(f[a]) > fabs(f[-a-b]) ) x = z[-a-b]+_T_/N;
	else goto bis;
	if( fabs(x-z[-a-b]) > fabs(z[b]-z[-a-b])/2.0 ) 
bis:
 	x = (z[b]+z[-a-b])/2.0;
	z[a]=x; *xx = x; f[a]=FUNC(X);
	if( SIGN(f[a]) == SIGN(f[b]) ) {
		b = -a-b;
		if( p < 6 ) p=0;
	} else p += 1;
	a = -a-b;	
	if( fabs(z[a]-z[b]) > eps && fabs(z[a]-z[-a-b]) > eps )
		if(p < 6) goto hyp; else goto bis;

out:
	free_dvector(z,-1,1);
	free_dvector(f,-1,1);
	return x;
}
*/
/******************************************************************************/
/*									      */
/******************************************************************************/

#undef CA
#undef PIO2
#undef SIGN
