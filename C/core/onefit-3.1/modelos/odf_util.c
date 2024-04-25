#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "struct.h"

#define EPS1	3.0e-11
#define EPS	8.0e-5
#define	JMAX	20
#define	JMAXP	JMAX+1
#define	K	5

/*****************************************************************************/
/*                              ODF_UTIL.C                                   */
/*****************************************************************************/
int	r_n_par(x)
Function	*x;
{
	return( (*x).n_par );
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	wsval(X)
Function *X;
{
	double	r_pval();
	int	r_n_par();
	int	i;

	for(i=0; i<r_n_par(X); i++) printf("p[%d]=%g\n",i,r_pval(X,i));
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_f_ptr(x,f)
double		(*f)();
Function	*x;
{
	(*x).f_ptr = f;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_plow(x,n)
int		n;
Function	*x;
{
	return( (*x).par[n].low_v);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_plow(x,n,lv)
int		n;
double		lv;
Function	*x;
{
	(*x).par[n].low_v = lv;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_phigh(x,n)
int		n;
Function	*x;
{
	return( (*x).par[n].high_v);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_phigh(x,n,hv)
int		n;
double		hv;
Function	*x;
{
	(*x).par[n].high_v = hv;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_pstep(x,n)
int		n;
Function	*x;
{
	return( (*x).par[n].step_v);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_pstep(x,n,sv)
int		n;
double		sv;
Function	*x;
{
	(*x).par[n].step_v = sv;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_pval(x,n)
int		n;
Function	*x;
{
	return( (*x).par[n].val);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_pval(x,n,v)
int		n;
double		v;
Function	*x;
{
	(*x).par[n].val = v;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	clear_struct(f_struct,n_par)

Function	*f_struct;
int	n_par;
{
	int	i;
	
	strcpy((*f_struct).name," ");
	(*f_struct).status 	=0;
	(*f_struct).n_par	=n_par;
	(*f_struct).f_ptr	=0;

	for (i=0; i<n_par; i++) {
			strcpy((*f_struct).par[i].name," ");
			(*f_struct).par[i].status	=0.0;
			(*f_struct).par[i].low_v	=0.0;
			(*f_struct).par[i].high_v	=0.0;
			(*f_struct).par[i].step_v	=0.0;
			(*f_struct).par[i].val  	=0.0;
			(*f_struct).par[i].min_v	=0.0;
	}



}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double smidpnt(X,p,n)
int	  p,n;
Function *X;
/*
This routine computes the n'th stage of refinement of an extended midpoint rule
X is input as a pointer to the structure Function to integrated between limits
of parameter p also input. When called with n=1, the routine returns the crudes
t an estimate of int(f(x)dx,a,b). Subsequent calls with n=2,3... (in that seque
ncial order) will improve the accuracy by adding (2/3)x3^(n-1) additional inter
ior points.
*/
{
	double	r_plow(),r_phigh();
	double 	a,b;
	double	tnm,sum,del,ddel;
	double	*s,*x,*it;
	int	j;

	x = &(*X).par[p].val;
	s = &(*X).par[p].min_v;
	it= &(*X).par[p].step_v;

	a = r_plow(X,p);
	b = r_phigh(X,p);
	
	if(a == b) return(0.0);

	if (n == 1) {
		*it=1;
		*x = 0.5*(a+b);
		*s=(b-a)*FUNC(X);
		return( *s );
	} 
	else {
		tnm = *it;
		del=(b-a)/(3.0*tnm);
		ddel=del+del;
		*x=a+0.5*del;
		sum=0.0;
		for (j=1;j<= *it;j++) {
			sum += FUNC(X);
			*x += ddel;
			sum += FUNC(X);
			*x += del;
		}
		*it *= 3;
		*s=(*s+(b-a)*sum/tnm)/3.0; 
		return( *s );
	}
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	sqromo(X,choose,p)
double	 (*choose)();
int	 p;
Function *X;
/*
Romberg integration on an open interval. Returns the integral of the function 
func from a to b, using any specified integrating routine choose and Romberg
method. Normally choose wil be an open formula, not evaluating yhe function at
the end points. It is assumed that choose triples the numder of steps on each
call, and that its error series contains only even powers of the number of 
steps. The routines midpnt and midinf among others ( see NR Book), are possible
choices for choose.
*/
{
	int	 j;
	double   ss,dss,h[JMAXP+1],s[JMAXP+1];
	void	 dpolint(),nrerror();

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
/*		printf("call to integrating routine no. %d\n",j);	     */
		s[j]=(*choose)(X,p,j);
		if (j >= K) {
			dpolint(&h[j-K],&s[j-K],K,0.0,&ss,&dss);
			if ( fabs(dss) < EPS*fabs(ss) ) return(ss);
		}
		s[j+1]=s[j];
		h[j+1]=h[j]/9.0;
	}
	nrerror("Too many steps in routine QROMO");
	return 0;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double sqgaus(X,p)
Function *X;
int	  p;
{
	int j;
	double a,b,xr,xm,dx,s,s1,s2;
	static double x[]={0.0,0.148874338981631,0.433395394129247,
		0.679409568299024,0.865063366688985,0.973906528517172};
	static double w[]={0.0,0.295524224714753,0.269266719309996,
		0.219086362515982,0.149451349150581,0.066671344308688};
	double	r_plow(),r_phigh();
	void	w_pval();

	a = r_plow(X,p);
	b = r_phigh(X,p);

	xm=0.5*(b+a);
	xr=0.5*(b-a);
	s=0;
	for (j=1;j<=5;j++) {
		dx=xr*x[j];
		w_pval(X,p,xm+dx);
		s1 = FUNC(X);
		w_pval(X,p,xm-dx);	
		s2 = FUNC(X);
		s += w[j]*(s1+s2);
	}
	return s *= xr;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double sqgausn(X,p,n)
Function *X;
int	  p,n;
{
	int j;
	double a,b,s,*x,*w;
	double	r_plow(),r_phigh(),*dvector();
	void	w_pval(),gauleg(),free_dvector();

	a = r_plow(X,p);
	b = r_phigh(X,p);

	x = dvector(0,n);
	w = dvector(0,n);
	gauleg(a,b,x,w,n);

	s=0;
	for (j=1;j<=n;j++) {
		w_pval(X,p,x[j]);
		s += w[j]*FUNC(X);
	}
	free_dvector(x,0,n);
	free_dvector(w,0,n);
	return s;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void gauleg(x1,x2,x,w,n)
double x1,x2,x[],w[];
int n;
{
	int m,j,i;
	double z1,z,xm,xl,pp,p3,p2,p1;

	m=(n+1)/2;
	xm=0.5*(x2+x1);
	xl=0.5*(x2-x1);
	for (i=1;i<=m;i++)  {
		z=cos(3.141592654*(i-0.25)/(n+0.5));
		do {
			p1=1.0;
			p2=0.0;
			for (j=1;j<=n;j++) {
				p3=p2;
				p2=p1;
				p1=((2.0*j-1.0)*z*p2-(j-1.0)*p3)/j;
			}
			pp=n*(z*p1-p2)/(z*z-1.0);
			z1=z;
			z=z1-p1/pp;
		} while (fabs(z-z1) > EPS1);
		x[i]=xm-xl*z;
		x[n+1-i]=xm+xl*z;
		w[i]=2.0*xl/((1.0-z*z)*pp*pp);
		w[n+1-i]=w[i];
	}
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
#undef 	EPS1	3.0e-11
#undef  EPS	8.0e-5
#undef 	JMAX	20
#undef	JMAXP	JMAX+1
#undef	K	5
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
