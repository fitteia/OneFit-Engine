/******************************************************************************/
/*					ODF.C				      */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "struct.h"
#include "struf.h"

#define	pi	3.141592653589793238
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

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
	m_ = sqrt(2*MAX(n-R*R*Ka3*Za*Za,1e-28));
	
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
double	int_godfSN_v1(X)
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
double	godfSN_v1(alfa,w,Wc,Ws,Ka3,K13,eta1a,R,BD)
double	alfa,w,Wc,Ws,Ka3,K13,eta1a,R,BD;
{
	Function X;
	double	af;
	double	int_godfSN_v1(),sqromo(),smidpnt(),sqgaus(),sqgausn();
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
	w_f_ptr(&X,int_godfSN_v1);

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
double	JodfSN_v1(BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,w)
double	BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,w;
{
	double godfSN_v1();
	double af;

	af =(	 godfSN_v1(1.0,w,wcM,ws,K13,K13,1.0,RM,BD)
	 	-godfSN_v1(1.0,w,wcm,ws,K13,K13,1.0,Rm,BD)	
	    )*sqrt(K13) +(
		 godfSN_v1(2.0,w,wcM,ws,K23,K13,eta12,RM,BD)
		-godfSN_v1(2.0,w,wcm,ws,K23,K13,eta12,Rm,BD)	
	    )*K13/K23*sqrt(K13/eta12);

	return 0.5*sqrt(2*pi/w)*af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfSN_v1(w,delta,K13,K23,eta12,RM,Rm,wcM,wcm,ws,BD)
double	w,delta,K13,K23,eta12,RM,Rm,wcM,wcm,ws,BD;
{
	double JodfSN_v1(),fh();
	double	af;

	af= fh(1,delta)*JodfSN_v1(BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,w)+
	    fh(2,delta)*JodfSN_v1(BD,K13,K23,eta12,RM,Rm,wcM,wcm,ws,2*w);
/*	printf("%lf\n",af);	*/
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	int_godfSN(X)
Function *X;
{
	double	alfa,w,Wc,Wb,Ka1,K31,eta1a,R,Q;
	double	A,a,n,m,m_,u;
	double	Wcw,Wbw,Z2Ro2,L,af;
	double	godfa();
	
	alfa	= r_pval(X,0); 
	w 	= r_pval(X,1);
	Wc	= r_pval(X,2);
	Wb	= r_pval(X,3);
	Ka1	= r_pval(X,4);
	K31	= r_pval(X,5);
	eta1a	= r_pval(X,6);
	R	= r_pval(X,7);
	Q	= r_pval(X,8);
	u	= r_pval(X,9);

	Wcw   = Wc/w;
	Wbw   = Wb/w;

	A = sqrt(Ka1*eta1a+(K31*eta1a-Ka1*eta1a)*u*u);
	L = A * sqrt(R*R*Wcw/(1.0+(R*R-1.0)*u*u));
/*
	Z2Ro2 = 1.0/(R*R);	
	L = A * sqrt(Wcw/(Z2Ro2+(1-Z2Ro2)*u*u));
*/

	if(u != 0.0){
		if(alfa != 2.0) 
			a = sqrt(Wbw*eta1a*u*u/((alfa==1.0)*(1-u*u)+Q*u*u));
		else    a = sqrt(Wbw*eta1a/Q);
	}
	else a = 0.0;
	
	n = sqrt(pow(a,4.0)+1.0);
	m = sqrt(2*(n+a*a));
	m_= sqrt(2*MAX(n-a*a,1e-14));

	if(A == 0.0) af=0.0;
	else af= -godfa(L,n,m_,m)/pow(A,3.0);
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	godfSN(alfa,w,Wc,Wb,Ka1,K31,eta1a,R,Q)
double	alfa,w,Wc,Wb,Ka1,K31,eta1a,R,Q;
{
	Function X;
	double	af;
	double	int_godfSN(),sqromo(),smidpnt(),sqgaus(),sqgausn();
	void	clear_struct();

	clear_struct(&X,10);

	w_pval(&X,0,alfa);
	w_pval(&X,1,w);
	w_pval(&X,2,Wc);
	w_pval(&X,3,Wb);
	w_pval(&X,4,Ka1);
	w_pval(&X,5,K31);
	w_pval(&X,6,eta1a);
	w_pval(&X,7,R);
	w_pval(&X,8,Q);
	w_plow(&X,9,0.0);
	w_phigh(&X,9,1.0);
	w_f_ptr(&X,int_godfSN);

	if(alfa==2.0 && Q==0.0) af=0.0; 
	else af= sqrt(2.0)/pi*sqgaus(&X,9);
/* 
	printf("%le",af);
	af= sqrt(2.0)/pi*sqromo(&X,smidpnt,9);	
	printf("%le\n",af);
*/
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	JodfSN(Q,K31,K21,eta12,RM,Rm,wcM,wcm,wb,w)
double	Q,K31,K21,eta12,RM,Rm,wcM,wcm,wb,w;
{
	double godfSN();
	double af;

	af =(	 godfSN(1.0,w,wcM,wb,1.0,K31,1.0,RM,Q)
	 	-godfSN(1.0,w,wcm,wb,1.0,K31,1.0,Rm,Q)	
	    )+(
		 godfSN(2.0,w,wcM,wb,K21,K31,eta12,RM,Q)
		-godfSN(2.0,w,wcm,wb,K21,K31,eta12,Rm,Q)	
	    )*eta12;

	return 0.5*sqrt(2*pi/w)*af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfSN(w,delta,K31,K21,eta12,RM,Rm,wcM,wcm,wb,Q)
double	w,delta,K31,K21,eta12,RM,Rm,wcM,wcm,wb,Q;
{
	double JodfSN(),fh();
	double	af;
	
	af = fh(1,delta)*JodfSN(Q,K31,K21,eta12,RM,Rm,wcM,wcm,wb,w)+
	     fh(2,delta)*JodfSN(Q,K31,K21,eta12,RM,Rm,wcM,wcm,wb,2*w);
/*	printf("%lf\n",af);	*/
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	odfS_K1B(w,delta,wcM,wcm,wb)
double	w,delta,wcM,wcm,wb;
{
	double JodfS_K1B(),fh();
	double	af;
	
	af = fh(1,delta)*JodfS_K1B(wcM,wcm,wb,w)+
	     fh(2,delta)*JodfS_K1B(wcM,wcm,wb,2*w);
/*	printf("%lf\n",af);	*/
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
double	JodfS_K1B(wcM,wcm,wb,w)
double	w,wcM,wcm,wb;
{
	double	af;
	
	af = 0.25*sqrt(pi/wb)*(   
		  log(2*sqrt(wb*wcM)/w+sqrt(4*wb*wcM/w/w+1) ) 
		- log(2*sqrt(wb*wcm)/w+sqrt(4*wb*wcm/w/w+1) )
	     );
/*	printf("%lf\n",af);	*/
	return af;
}
/******************************************************************************/
/*									      */
/******************************************************************************/
#undef MAX
#undef MIN
