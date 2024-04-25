/******************************************************************************/
/*				ROT.C 				   	      */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define  CR    1.7086248e11   /* 3/2*gama^4*(h/2pi)^2*(4/15)*10^48*I*(I+1)*/
			      /* para I=1/2 (protao)                      */
			      /* multiplicar por <1/r^6> em Angstron^-6 */
			      /* programa afactor caso isotropo         */
#define  CR2   6.407343e11    /* idem sem o factor 4/15                 */

double	rot(t,w,delta)
double	t,w,delta;
{
	double	lor1,lor2,J1wd,J2wd,af;
	double 	lor(),fij();
	
	lor1 = lor(t,w);
	lor2 = lor(t,2*w);

	if(delta == 0.0) af = CR*(lor1+4*lor2);
	else {
		J1wd = 	fij(1,0,delta)*6*lor1 +
		      	fij(1,1,delta)*lor1   +
		       	fij(1,2,delta)*4*lor1;
		J2wd = 	fij(2,0,delta)*6*lor2 +
		 	fij(2,1,delta)*lor2   +
			fij(2,2,delta)*4*lor2;
		af = CR * (J1wd + J2wd);
	}
	return af;
}
/******************************************************************************/
/*								   	      */
/******************************************************************************/
double	Pij(i,j,S,P4)
int	i,j;
double	S,P4;
{
	double	af;
	void	nrerror();
	
	i=abs(i);
	j=abs(j);
	switch(i+j){
		case 0:
	 		af = 7+10*S+18*P4-35*S*S;
		  /*af = 7+10*S+18*P4;*/
			break;
		case 1:
			af = 7+5*S-12*P4;
			break;
		case 2:
			if(i==1 & j==1) af = 0.5*(14+5*S+16*P4);
			else af = 7-10*S+3*P4;
			break;
		case 3:
			af = 7-5*S-2*P4;
			break;
		case 4:
			af = 0.5*(14+20*S+P4);
			break;
		default:
			nrerror("Pij not defined!");
	}
	return af/7.0;
}
/******************************************************************************/
/*								   	      */
/******************************************************************************/
double JR(k,w,S,P4,ts,tl,A0,A1,A2)
int	k;
double	w,S,P4,ts,tl,A0,A1,A2;
{
	double af,Pij(),lor(),t0,t1,t2;
	void   nrerror();

	t0 = ts;
	t1 = 6.0/(1.0/tl+5.0/ts);
	t2 = 6.0/(4.0/tl+2.0/ts);

	af = 	A0*Pij(k,0,S,P4)*lor(t0,w)
	      + A1*Pij(k,1,S,P4)*lor(t1,w)
	      + A2*Pij(k,2,S,P4)*lor(t2,w);
	
	switch(k){
		case 0:
			return 24*af/15.0;
			break;
		case 1:
			return 4*af/15.0;
			break;
		case 2:
			return 16*af/15.0;
			break;
		default:
			nrerror("JR not defined!");
	}
}
/******************************************************************************/
/*								   	      */
/******************************************************************************/
double	rot_reor(ts,tl,w,delta,S,P4,A0,A1,A2)
double	ts,tl,w,delta,S,P4,A0,A1,A2;
{
	double	J1wd,J2wd,af;
	double 	fij(),JR();

	J1wd = 	fij(1,0,delta)*JR(0,w,S,P4,ts,tl,A0,A1,A2)+
	      	fij(1,1,delta)*JR(1,w,S,P4,ts,tl,A0,A1,A2)+
	       	fij(1,2,delta)*JR(2,w,S,P4,ts,tl,A0,A1,A2);
	J2wd = 	fij(2,0,delta)*JR(0,2*w,S,P4,ts,tl,A0,A1,A2)+
	 	fij(2,1,delta)*JR(1,2*w,S,P4,ts,tl,A0,A1,A2)+
		fij(2,2,delta)*JR(2,2*w,S,P4,ts,tl,A0,A1,A2);
	af = J1wd + J2wd;
	return af;
}
/******************************************************************************/
/*								   	      */
/******************************************************************************/
double	rot_reor_poli(ts,tl,w,S,P4,A0,A1,A2)
double	ts,tl,w,S,P4,A0,A1,A2;
{
	double	J1wd,J2wd,af;
	double 	fij(),JR();

	J1wd = 	1./30.*JR(0,w,S,P4,ts,tl,A0,A1,A2)+
	      	2./5. *JR(1,w,S,P4,ts,tl,A0,A1,A2)+
	       	0.1   *JR(2,w,S,P4,ts,tl,A0,A1,A2);
	J2wd = 	2./15.*JR(0,2*w,S,P4,ts,tl,A0,A1,A2)+
	 	8./5. *JR(1,2*w,S,P4,ts,tl,A0,A1,A2)+
		2./5. *JR(2,2*w,S,P4,ts,tl,A0,A1,A2);
	af = J1wd + J2wd;
/*	printf("\nJ0rot=       %lg",JR(0,w,S,P4,ts,tl,A0,A1,A2));
	printf("\nJ1rot=       %lg",JR(1,w,S,P4,ts,tl,A0,A1,A2));
	printf("\nJ2rot=       %lg",JR(2,w,S,P4,ts,tl,A0,A1,A2));
	printf("\nJ1wpoli=     %lg",J1wd);	
	printf("\nJ2wpoli=     %lg",J2wd);
	printf("\(1/T1)R poli= %lg",af);	*/
/*
printf("\ncorpo    f=%3.1e J1(w)=%g J2(2w)=%g 1/T1=%g",w/6.283185307,
CR2*0.2*J1wd,CR2*0.2*J2wd,CR2*0.2*(J1wd+J2wd));
*/

	return af;
}
/******************************************************************************/
/*								   	      */
/******************************************************************************/
double	rot_reor_ro(ts,tl,w1,w,delta,S,P4,A0,A1,A2)
double	ts,tl,w,w1,delta,S,P4,A0,A1,A2;
{
	double	J0wd,J1wd,J2wd,af;
	double 	fij(),JR();

	J0wd = 	fij(0,0,delta)*JR(0,2*w1,S,P4,ts,tl,A0,A1,A2)+
	      	fij(0,1,delta)*JR(1,2*w1,S,P4,ts,tl,A0,A1,A2)+
	       	fij(0,2,delta)*JR(2,2*w1,S,P4,ts,tl,A0,A1,A2);
	J1wd = 	fij(1,0,delta)*JR(0,w,S,P4,ts,tl,A0,A1,A2)+
	      	fij(1,1,delta)*JR(1,w,S,P4,ts,tl,A0,A1,A2)+
	       	fij(1,2,delta)*JR(2,w,S,P4,ts,tl,A0,A1,A2);
	J2wd = 	fij(2,0,delta)*JR(0,2*w,S,P4,ts,tl,A0,A1,A2)+
	 	fij(2,1,delta)*JR(1,2*w,S,P4,ts,tl,A0,A1,A2)+
		fij(2,2,delta)*JR(2,2*w,S,P4,ts,tl,A0,A1,A2);
	af = (J0wd + 10*J1wd + J2wd)/4.0;
	return af;
}
/******************************************************************************/
/*								   	      */
/******************************************************************************/

double rot_reor_cad_poli(w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2)

double	w;
double	tauS,tauL;	/* tempos de correlacao p/ rot corpo */
double	S,P4;		/* parametros de ordem p/ corpo      */
double	tauSc,tauLc;	/* tempos de correlacao p/ rot cadeia */
double	Sc,P4c;		/* parametros de ordem p/ cadeia (relativos ao corpo) */
double	A0,A1,A2;       /* "Afactors" */
{
double	Jcad();
double	J1w;
double	J2w;

J1w=
1./30. * Jcad(0,w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2)+
2./5.  * Jcad(1,w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2)+
1./10. * Jcad(2,w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2);

J2w=
2./15. * Jcad(0,2.*w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2)+
8./5.  * Jcad(1,2.*w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2)+
2./5.  * Jcad(2,2.*w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2);
/*
printf("\ncadeias  f=%3.1e J1(w)=%g J2(2w)=%g 1/T1=%g",w/6.283185307,
CR2*0.8*J1w,CR2*0.8*J2w,CR2*0.8*(J1w+J2w));
*/
return	(J1w+J2w);
}

/******************************************************************************/

double	Jcad(k,w,tauS,tauL,S,P4,tauSc,tauLc,Sc,P4c,A0,A1,A2)

int	k;
double	w;
double	tauS,tauL;	/* tempos de correlacao p/ rot corpo */
double	S,P4;		/* parametros de ordem p/ corpo      */
double	tauSc,tauLc;	/* tempos de correlacao p/ rot cadeia */
double	Sc,P4c;		/* parametros de ordem p/ cadeia (relativos ao corpo) */
double	A0,A1,A2;       /* "Afactors" */
{
int	i,j;
double	tau[3][3];
double	tauM[3];
double	tauC[3];
double	C[3][3];
double	B[3][3];
double	AB[3];
double	Jwk;
double	lor();
double	d2ij2();

tauM[0]=tauS;
tauM[1]=1./(1./(6.*tauL)+5./(6.*tauS));
tauM[2]=1./(2./(3.*tauL)+1./(3.*tauS));
tauC[0]=tauSc;
tauC[1]=1./(1./(6.*tauLc)+5./(6.*tauSc));
tauC[2]=1./(2./(3.*tauLc)+1./(3.*tauSc));

for(i=0; i<3; i++){
	for(j=0; j<3; j++){
		tau[i][j]=tauM[i]*tauC[j]/(tauM[i]+tauC[j]);
		B[i][j]=d2ij2(i,j,Sc,P4c)*lor(tau[i][j],w);
		if(i>0) B[i][j]=B[i][j]*2.;
		}
	}
AB[0]=B[0][0]*A0+B[0][1]*A1+B[0][2]*A2;
AB[1]=B[1][0]*A0+B[1][1]*A1+B[1][2]*A2;
AB[2]=B[2][0]*A0+B[2][1]*A1+B[2][2]*A2;

switch(k){
	case 0:
		for(j=0; j<3; j++){
			C[0][j]=d2ij2(0,j,S,P4);
			}
		Jwk=8.*(C[0][0]*AB[0]+C[0][1]*AB[1]+C[0][2]*AB[2]);
		break;
	case 1:
		for(j=0; j<3; j++){
			C[1][j]=d2ij2(1,j,S,P4);
			}
		Jwk=4./3.*(C[1][0]*AB[0]+C[1][1]*AB[1]+C[1][2]*AB[2]);
		break;
	case 2:
		for(j=0; j<3; j++){
			C[2][j]=d2ij2(2,j,S,P4);
			}
		Jwk=16./3.*(C[2][0]*AB[0]+C[2][1]*AB[1]+C[2][2]*AB[2]);
		break;
	}
return Jwk;
}


/******************************************************************************/

double	d2ij2(i,j,S,P4)

int	i,j; /* linha e coluna respectivamente */
double	S,P4;
{
int	k;
double	d2;

k=i+j;

switch(k){
	case 0:
		d2=7.+10.*S+18.*P4;
		break;
	case 1:
		d2=7.+5.*S-12.*P4;
		break;
	case 2:
		if(i==0 || j==0){
			d2=7.-10.*S+3.*P4;
			}
		else{
			d2=0.5*(14.+5.*S+16.*P4);
			}
		break;
	case 3:
		d2=7.-5.*S-2.*P4;
		break;
	case 4:
		d2=0.5*(14.+20.*S+P4);
	}
return (d2/35.);
}
/******************************************************************************/
/*								   	      */
/******************************************************************************/


