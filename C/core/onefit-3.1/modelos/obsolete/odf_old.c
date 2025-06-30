/******************************************************************************/
/*				ODF_OLD.C				      */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "[pedros.c]struct.h"
#include "[pedros.c]struf.h"

#define	pi	3.141592653589793238

/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfN(wcm,wcM,w,delta)
double	wcm,wcM,w,delta;
{
	double	af,am,aM;
	double	JodfN(),fh();

	am = wcm/w;
	aM = wcM/w;

	if(delta == 0.0) af = JodfN(w,aM,am);
	else af = fh(1,delta)*JodfN(w,aM,am) + fh(2,delta)*JodfN(2*w,aM/2,am/2);
	return af;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfN_ro(wcm,wcM,w,w1,delta)
double	wcm,wcM,w,w1,delta;
{
	double	af,am,aM,a1M,a1m;
	double	JodfN(),fh();

	am  = wcm/w;
	aM  = wcM/w;
	a1m = wcm/w1;
	a1M = wcM/w1;

	af = 0.25 * fh(0,delta)*JodfN(2*w1,a1M/2,a1m/2) 
	   + 2.5  * fh(1,delta)*JodfN(w,aM,am)
	   + 0.25 * fh(2,delta)*JodfN(2*w,aM/2,am/2);
	return af;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	JodfN(w,aM,am)
double	w,aM,am;
{
	double	godfN();
	return sqrt(2*pi/w)*(godfN(aM) - godfN(am));
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfSa(wcm,wcM,w,delta)
double	wcm,wcM,w,delta;
{
	double	af,am,aM;
	double 	JodfSa(),fh();

	am = wcm/w;
	aM = wcM/w;

	if(delta == 0.0) af = JodfSa(w,aM,am);
	else af=fh(1,delta)*JodfSa(w,aM,am)+fh(2,delta)*JodfSa(2*w,aM/2,am/2);	
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfSa_ro(wcm,wcM,w,w1,delta)
double	wcm,wcM,w,w1,delta;
{
	double	af,am,aM,a1M,a1m;
	double 	JodfSa(),fh();

	am  = wcm/w;
	aM  = wcM/w;
	a1m = wcm/w1;
	a1M = wcM/w1;

	af = 0.25 * fh(0,delta)*JodfSa(2*w1,a1M/2.0,a1m/2.0)
	   + 2.5  * fh(1,delta)*JodfSa(w,aM,am)	
	   + 0.25 * fh(2,delta)*JodfSa(2*w,aM/2,am/2);	
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	JodfSa(w,aM,am)
double	w,aM,am;
{
	double	godfSa();
	return 2*pi/w*(godfSa(aM) - godfSa(am));
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	godfN(a)
double 	a;
{
	double 	af;
	double	r2a,atanh();

	r2a = sqrt(2*a);

	af =  (atan(r2a+1) + atan(r2a-1) - atanh(r2a/(a+1)))/pi;	
/*	af = -(atan(r2a+1) + atan(r2a-1) + atanh(r2a/(a+1)))/pi;	*/
	return af;
}

/******************************************************************************/
/*									      */
/******************************************************************************/
double	godfSa(a)
double 	a;
{
	double 	af;

	af = 2/pi*atan(a);	
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	godf_k123(wc_w,eta1a,Ka3,R)
double	wc_w,eta1a,Ka3,R;
{
	double	Za,B,b,n,m,m_,af,a1,r2Za;
	double	godfa(),atanh();

	Za   = sqrt(eta1a*wc_w);
	r2Za = sqrt(2.0)*Za;
	B  = sqrt(fabs(b=1.0-Ka3*R*R));
	n  = sqrt(pow(R*Za,4.0)*Ka3*Ka3+1.0);
	m  = sqrt(2*(n+R*R*Ka3*Za*Za));
	m_ = sqrt(2*(n-R*R*Ka3*Za*Za));
	
	a1 = (atan(r2Za+1.0)+atan(r2Za-1.0)-atanh(r2Za/(Za*Za+1.0)))/pi;
	if (b==0.0)    	af = a1;
	else if(b>0.0) 	af = a1+sqrt(2.0)/pi/B*godfa(Za*B,n,m_,m);
	else 		af = a1-sqrt(2.0)/pi/B*godfa(Za*B,n,m,m_);
	/* printf("%lf\n",af); */
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	godfa(ZaB,n,m1,m2)
double	ZaB,n,m1,m2;
{
	double af;
	double	atanh();

	af = atanh(ZaB*m1/(ZaB*ZaB+n))/m1
	     -( atan((2*ZaB-m1)/m2) + atan((2*ZaB+m1)/m2) )/m2;
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	Jodf_k123(K13,K23,eta12,RM,Rm,wcM,wcm,w)
double	K13,K23,eta12,RM,Rm,wcM,wcm,w;
{
	double godf_k123();
	double	aM,am,af;

	aM = wcM/w; am = wcm/w;
	af = (godf_k123(aM,1.0,K13,RM) - godf_k123(am,1.0,K13,Rm))*sqrt(K13) 
	   + (godf_k123(aM,eta12,K23,RM)-godf_k123(am,eta12,K23,Rm))*K13/K23
	     *sqrt(K13/eta12);
	return 0.5*sqrt(2*pi/w)*af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	odf_k123(w,delta,K13,K23,eta12,RM,Rm,wcM,wcm)
double	w,delta,K13,K23,eta12,RM,Rm,wcM,wcm;
{
	double Jodf_k123(),fh();
	double	af;

	af = 	fh(1,delta)*Jodf_k123(K13,K23,eta12,RM,Rm,wcM,wcm,w)+
		fh(2,delta)*Jodf_k123(K13,K23,eta12,RM,Rm,wcM,wcm,2*w);
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	int_godfSN(X)
Function *X;
{
	double	alfa,w,Wc,Ws,Ka3,eta1a,R,Q;
	double	A,a,n,m,m_,u;
	double	Wca,Wsa,Z2Ro2,af;
	double	godfa();
	
	alfa	= r_pval(X,0); 
	w 	= r_pval(X,1);
	Wc	= r_pval(X,2);
	Ws	= r_pval(X,3);
	Ka3	= r_pval(X,4);
	eta1a	= r_pval(X,5);
	R	= r_pval(X,6);
	Q	= r_pval(X,7);
	u	= r_pval(X,8);

	Wca   = Wc*eta1a/w;
	Wsa   = Ws*eta1a/w;
	Z2Ro2 = 1.0/(Ka3*R*R);
	a     = sqrt(Wsa*Q*u*u/((alfa==1.0)*(1-u*u)+Q*u*u));

	A = sqrt(Wca/(Z2Ro2+(1-Z2Ro2)*u*u));
	n = sqrt(pow(a,4.0)+1.0);
	m = sqrt(2*(n+a*a));
	m_= sqrt(2*(n-a*a));

	af= -godfa(A,n,m_,m);
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	godfSN(alfa,w,Wc,Ws,Ka3,K13,eta1a,R,BD)
double	alfa,w,Wc,Ws,Ka3,K13,eta1a,R,BD;
{
	Function X;
	double	af;
	double	sqromo(),smidpnt(),sqgaus(),sqgausn();
	void	clear_struct();

	clear_struct(&X,9);

	w_pval(&X,0,alfa);
	w_pval(&X,1,w);
	w_pval(&X,2,Wc);
	w_pval(&X,3,Ws);
	w_pval(&X,4,Ka3);
	w_pval(&X,5,eta1a);
	w_pval(&X,6,R);
	w_pval(&X,7,BD*K13);
	w_plow(&X,8,0.0);
	w_phigh(&X,8,1.0);
	w_f_ptr(&X,int_godfSN);

	af= sqrt(2.0)/pi*sqgaus(&X,8);
/* 
	printf("%le",af);
	af= sqrt(2.0)/pi*sqromo(&X,smidpnt,8);	
	printf("%le\n",af);
*/
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	JodfSN(BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,w)
double	BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,w;
{
	double godfSN();
	double af;

	af =(	 godfSN(1.0,w,wcM,ws,K13,K13,1.0,RM,BD)
	 	-godfSN(1.0,w,wcm,ws,K13,K13,1.0,Rm,BD)	
	    )*sqrt(K13) +(
		 godfSN(2.0,w,wcM,ws,K23,K13,eta12,RM,BD)
		-godfSN(2.0,w,wcm,ws,K23,K13,eta12,Rm,BD)	
	    )*K13/K23*sqrt(K13/eta12);

	return 0.5*sqrt(2*pi/w)*af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfSN(w,delta,K13,K23,eta12,RM,Rm,wcM,wcm,ws,BD)
double	w,delta,K13,K23,eta12,RM,Rm,wcM,wcm,ws,BD;
{
	double JodfSN(),fh();
	double	af;

	af = 	fh(1,delta)*JodfSN(BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,w)+
		fh(2,delta)*JodfSN(BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,2*w);
/*	printf("%lf\n",af);	*/
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
