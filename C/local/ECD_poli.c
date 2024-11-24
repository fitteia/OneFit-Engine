#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

#define pi 3.141592653589793238
#define		FILE_J1TAB	"/home/lfx/lib/j1tabodf.dat"
#define		FILE_D2J1TAB	"/home/lfx/lib/d2J1tab.dat"
#define		TABSIZE_R	90
#define		TABSIZE_fr	72

int	flag=0;  /* flag=0 primeira vez, flag!=0 vezes seguintes */
		 /* flag!=0 nao carrega tabela			 */

double	J1tab[TABSIZE_R][TABSIZE_fr];
double	J1drv2tab[TABSIZE_R][TABSIZE_fr];
double	tabfr[TABSIZE_fr],tabR[TABSIZE_R];


double CED_poli(double f, double Aecd, double fcM, double R, double v0)
{
  double ECD_poli();
  return ECD_poli(f, Aecd, 0.0, fcM, R, v0);
}

double ECD_poli(double f, double Aecd, double fcm, double fcM, double R, double v0)
{

        double  J1odfcolpoli();
        double  af,wr,wcm,wcM,w;

        wcm = 2*pi*fcm;
        wcM = 2*pi*fcM;

        w   = 2*pi*f;
        wr  = w/wcM;

        af = Aecd*J1odfcolpoli(wr,R,v0);

      return af;
}

double	J1odfcolpoli(wr,R,v0)

double	wr,R,v0;
{
double	J1odfcol();
double	J1odf_low2();

/* return(2./5. * J1odfcol(wr,R) + 8./5. * J1odfcol(2.*wr,R)); */
return(2./5. * J1odf_low2(1.,R,wr,v0) + 8./5. * J1odf_low2(1.,R,2.*wr,v0));
}


double	J1odfcol(wr,R)

double	wr,R;
{
void read_Jtab();
int itab,jtab;
double fr1,fr2,R1,R2,J1R1,J1R2;
int fcerto;
int Rcerto;
int interpol; /* interpol:  0 -> interpolar em f e em R
				1 -> f exacto - interpolar so em R
				2 -> R exacto - interpolar so em f
				3 -> f e R exactos - interpolar em f e em R  */

double	*J1_pt1,*J1_pt2,*J1drv2_pt1,*J1drv2_pt2;
double	J1odf_low();
double	J1odf_lowR();

if(flag==0){
	read_Jtab(FILE_J1TAB,tabfr,tabR,J1tab);
	read_Jtab(FILE_D2J1TAB,tabfr,tabR,J1drv2tab);
	flag=flag+1;
	}
/*
printf("R= %le, wr= %le\n",R,wr);
*/

if(wr*wr/R < 1.0e-8){
	/*
	printf("wr*wr/R=%le\n",wr*wr/R);
	*/
	return(J1odf_low(1.,R,wr));
	}

fcerto=0;
Rcerto=0;
interpol=0; 
fr1=0.;
fr2=0.;
R1=0.;
R2=0.;
itab=1;
jtab=1;
/*
printf("tabR[0]=%le\n",tabR[0]);
*/
if(R<tabR[0]){
	printf("\nErro!, R fora dos limites da tabela");
	}
if(wr<tabfr[0]){
	printf("\n!, wr fora dos limites da tabela wr*wr/R= %le\n",wr*wr/R);
	return(J1odf_low(1.,R,wr));
/*	exit(1);     */
	}

while(1){
	R2=tabR[itab];
	/*
	printf("R2= %le\n",R2);
	*/
	if(R==R2){
		Rcerto=1;
		break;
		}
	if(R<R2){
		R1=tabR[itab-1];
		break;
		}
	itab=itab+1;
	if(itab==TABSIZE_R){
		printf("\nErro!, R fora dos limites da tabela (maior)");
		exit(1);
		}
	}

while(1){
	fr2=tabfr[jtab];
	/*
	printf("fr2= %le\n",fr2);
	*/
	if(wr==fr2){ 
		fcerto=1;
		break;
		}
	if(wr<fr2){
		fr1=tabfr[jtab-1];
		break;
		}
	if(jtab==TABSIZE_fr){
		return(J1odf_low(1.,R,wr)); /* a funcao aprox. p/ baixas */
		}			    /* freq. tem uma assimptota  */
	jtab=jtab+1;			    /* em 1/w^2 que coincide com */
	}				    /* a assimptota a alta freq. */


if      (!fcerto && !Rcerto) interpol = 0;
else if ( fcerto && !Rcerto) interpol = 1;
else if (!fcerto &&  Rcerto) interpol = 2;
else if ( fcerto &&  Rcerto) interpol = 3;

switch(interpol){
	case 0: /* f e R intermedios relativamente aos elementos da tabela */
		/*
		printf("f e R intermedios\n");
		*/
		J1_pt1=(&J1tab[itab-1][0])-1;
		J1_pt2=(&J1tab[itab][0])-1;
		J1drv2_pt1=(&J1drv2tab[itab-1][0])-1;
		J1drv2_pt2=(&J1drv2tab[itab][0])-1;

		dsplint(tabfr-1,J1_pt1,J1drv2_pt1,TABSIZE_fr,wr,&J1R1);
		dsplint(tabfr-1,J1_pt2,J1drv2_pt2,TABSIZE_fr,wr,&J1R2);

		return(J1R1+(R-R1)/(R2-R1)*(J1R2-J1R1));

	case 1: /* f certo R intermedio */
		/*
		printf("f certo e R intermedio\n");
		*/
		J1R1=J1tab[itab-1][jtab];
		J1R2=J1tab[itab][jtab];
		return(J1R1+(R-R1)/(R2-R1)*(J1R2-J1R1));

	case 2: /* f intermedio e R certo */
		/*
		printf("f intermedio e R certo\n");
		*/
		J1_pt2=(&J1tab[itab][0])-1;
		J1drv2_pt2=(&J1drv2tab[itab][0])-1;

		dsplint(tabfr-1,J1_pt2,J1drv2_pt2,TABSIZE_fr,wr,&J1R2);
		return(J1R2);		

	case 3: /* f e R certos */
		/*
		printf("f e R certos\n");
		*/
		return(J1tab[itab][jtab]);
	}
}
/*** Le tabelas de J1odf ou das respectivas  2as derivadas ***/

void read_Jtab(tabfile,tabfr,tabR,J1tab)

char	*tabfile;	/* nome da file que contem a tabela */
double	*tabfr,*tabR;	/* arays com os valores de fr e R   */
double	*J1tab;		/* matriz com os valores de J1 (ou das 2as derivadas) */
{
FILE	*fp,*openf();
int	nR,nfr;		/* numero total de R's e f's */
int	iR,ifr;		/* contador de R's e f's     */

fp = openf(tabfile,"r");

 if (!fscanf(fp,"%d %d",&nR,&nfr)) printf("fscanf() call error in ECD_poli.c, read_Jtab()\n");
/*
printf("nR=%d,nfr=%d\n",nR,nfr);
*/
for(iR=0; iR<nR; iR++){
  if (!fscanf(fp,"%le",tabR+iR)) printf("fscanf() call error in ECD_poli.c, read_Jtab() for loop iR\n");
	/*
	printf("R= %le\n",tabR[iR]);
	*/
	for(ifr=0; ifr<nfr; ifr++){
	  if (!fscanf(fp,"%le %le",tabfr+ifr,J1tab+iR*nfr+ifr)) printf("fscanf() call error in ECD_poli.c, read_Jtab() for loop ifr\n");
		/* 
		printf("fr=%le J1=%le\n",tabfr[ifr],*(J1tab+iR*nfr+ifr));
		*/
		}
	}
fclose(fp);
}

/**** limite a baixas frequencias - dependente de frequencia ******/

double J1odf_low(A,R,wr)    

double A;  /*	Constante multiplicativa */
double R;         
double wr;
{
double a,C1,sqrt2,pol1,pol2,Jlow;

sqrt2 = sqrt(2.);

a=pow(R+wr*wr,0.25);
C1= sqrt2/(4. * a*a*a);
pol1=1 + sqrt2*a + a*a;
pol2=1 - sqrt2*a + a*a;

Jlow=A*2.*R*C1*(0.5*log(pol1/pol2)+atan(sqrt2/a+1)+atan(sqrt2/a-1));

return(Jlow); 
}


/** limite a baixas freq. - dependente de freq. (c/ freq. de corte inferior)**/

double J1odf_low2(A,R,wr,v0)    

double A;  /*	Constante multiplicativa */
double R;         
double wr;
double v0;
{
double a,C1,sqrt2,pol1,pol2,pol3,pol4,Jlow;

sqrt2 = sqrt(2.);

a=pow(R+wr*wr,0.25);
C1= sqrt2/(4. * a*a*a);
pol1=1 + sqrt2*a + a*a;
pol2=1 - sqrt2*a + a*a;
pol3=v0*v0 + sqrt2*a*v0 + a*a;
pol4=v0*v0 - sqrt2*a*v0 + a*a;  

Jlow=A*2.*R*C1*(0.5*(log(pol1/pol2)-log(pol3/pol4))
    +atan(sqrt2/a+1)+atan(sqrt2/a-1)-atan(sqrt2*v0/a+1)-atan(sqrt2*v0/a-1));
return(Jlow); 
}

/**** limite a baixo R - dependente de frequencia ******/

double J1odf_lowR(A,R,wr)    

double A;  /*	Constante multiplicativa */
double R;         
double wr;
{
double a,C1,sqrt2,pol1,pol2,Jlow;

sqrt2 = sqrt(2.);

a=pow(wr*wr,0.25);
C1= sqrt2/(4. * a*a*a);
pol1=1 + sqrt2*a + a*a;
pol2=1 - sqrt2*a + a*a;

Jlow=A*2.*R*C1*(0.5*log(pol1/pol2)+atan(sqrt2/a+1)+atan(sqrt2/a-1));

return(Jlow); 
}

double J1odf_high(A,R,wr)

double	A,R,wr;
{
return(2.*A*R/wr/wr);
}

/* auxliar code for ./tmp/tmp-.c */



















 /** XGFITN ADITIONAL CODE END **/
