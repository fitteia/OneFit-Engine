#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "struct.h"
#include "fitk_util.h"
#include "fitutil.h"

#define EPS	1.0e-4
#define	JMAX	14
#define	JMAXP	JMAX+1
#define	K	5

/*****************************************************************************/
/*                             FITK_UTIL.C                                   */
/*****************************************************************************/
int	r_n_par(Function *x)
// Function	*x;
{
	return( (*x).n_par );
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	wsval(Function *X)
// Function *X;
{
	int	i;

	for(i=0; i<r_n_par(X); i++) printf("p[%ld]=%lg\n",i,r_pval(X,i));
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_f_ptr(Function *x,double		(*f)(Function *a))
// double		(*f)();
// Function	*x;
{
	(*x).f_ptr = f;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_plow(Function *x, int n)
// int		n;
// Function	*x;
{
	return( (*x).par[n].low_v);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_plow(Function *x, int n, double lv)
// int		n;
// double		lv;
// Function	*x;
{
	(*x).par[n].low_v = lv;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_phigh(Function *x, int n)
// int		n;
// Function	*x;
{
	return( (*x).par[n].high_v);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_phigh(Function *x, int n, double hv)
// int		n;
// double		hv;
// Function	*x;
{
	(*x).par[n].high_v = hv;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_pstep(Function *x, int n)
// int		n;
// Function	*x;
{
	return( (*x).par[n].step_v);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_pstep(Function *x, int n, double sv)
// int		n;
// double		sv;
// Function	*x;
{
	(*x).par[n].step_v = sv;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double	r_pval(Function *x, int n)
// int		n;
// Function	*x;
{
	return( (*x).par[n].val);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	w_pval(Function *x, int n, double v)
// int		n;
// double		v;
// Function	*x;
{
	(*x).par[n].val = v;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void	clear_struct(Function *f_struct, int n_par)
// Function	*f_struct;
// int	n_par;
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
double smidpnt(Function *X, int p, int n)
// int	  p,n;
// Function *X;
/*
This routine computes the n'th stage of refinement of an extended midpoint rule
X is input as a pointer to the structure Function to integrated between limits
of parameter p also input. When called with n=1, the routine returns the crudes
t an estimate of int(f(x)dx,a,b). Subsequent calls with n=2,3... (in that seque
ncial order) will improve the accuracy by adding (2/3)x3^(n-1) additional inter
ior points.
*/
{
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
double	sqromo(Function *X, double (*choose)(Function *x), int p)
// double	 (*choose)();
// int	 p;
// Function *X;
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
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
#undef  EPS	1.0e-4
#undef 	JMAX	14
#undef	JMAXP	JMAX+1
#undef	K	5
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

