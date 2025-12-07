#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Florence_f.h"
#include "Florence_c.h"

/**
      SI      = 0.5
      GAMMAI  = 2.675222D+8
      SPIN    = 2.5
      IREL    = 1
      TAUS0M(J,1) = 0.0236
      TAURM(J,1)  = 2.88D-11
      TAUVM(J,1)  = 2.86D-12
      TAUMM(J,1)  = 0.D-6
      TAUDELTA = 2
      DPARAM(J) = 0.0
      EPARAM(J) = 0.0
      S4M(J)    = 0.0
      GXM(J)=1
      GYM(J)=1
      GZM(J)=1
      AXM(J) = 0.00
      AYM(J) = 0.00
      AZM(J) = 0.0
      DM(J)   = 0
      DDM(J)  = 0.0D-5
      CONCM(J)= 0.001
      ACQ = 1
      AMOLFRAM(J)= 12
      RKM(J)     = 2.767
      ACONTM(J)  = 0.748
      THETAM(J)  = 0
      PHIM(J)    = 0
**/

double FlorenceN(
	  double index,
	  double n,
      double FREQ,
      double SI,
      double GAMMAI,
      double SPIN,
      double IREL,
      double DELTA2,
      double TAURM,
      double TAUVM,
      double TAUMM,
      double DPARAM,
      double EPARAM,
      double S4M,
      double GXM,
      double GYM,
      double GZM,
      double AXM,
      double AYM,
      double AZM,
      double DM,
      double DDM,
      double CONCM,
      double ACQ,
      double AMOLFRAM,
      double RKM,
      double ACONTM,
      double THETAM,
      double PHIM
)
{
	static double R1[]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
   	static int count = 0;	
	double PINP[27];
    double aux;

	if (count < 1 || count > (int) n-1 ) {
		FREQ = log10(FREQ);

     	PINP[0] = SI;
    	PINP[1] = GAMMAI;
   		PINP[2] = SPIN;
      	PINP[3] = IREL;
      	PINP[4] = DELTA2; // TAUS0M
      	PINP[5] = TAURM;
      	PINP[6] = TAUVM;
      	PINP[7] = TAUMM;
      	PINP[8] = 2.0; // TAUDELTA // TO FORCE PINP[4] TO BE DELTA2
      	PINP[9] = DPARAM;
      	PINP[10] = EPARAM;
      	PINP[11] = S4M;
      	PINP[12] = GXM;
      	PINP[13] = GYM;
      	PINP[14] = GZM;
      	PINP[15] = AXM;
      	PINP[16] = AYM;
      	PINP[17] = AZM;
      	PINP[18] = DM;
      	PINP[19] = DDM;
      	PINP[20] = CONCM;
      	PINP[21] = ACQ;
      	PINP[22] = AMOLFRAM;
      	PINP[23] = RKM;
      	PINP[24] = ACONTM;
      	PINP[25] = THETAM;
      	PINP[26] = PHIM;

		florencef77_(PINP,&FREQ,R1);
		printf("reset count: %d\n%lg %lg %lg %lg %lg %lg %lg\n",count, FREQ,R1[0],R1[1],R1[2],R1[3],R1[4],R1[5]);
 		count = 0;
	}
	switch ( (int) index ) {
		default:
			aux = R1[0];
			break;
		case 1:
			count++;
			aux = R1[1];
			break;
		case 2:
			count++;
			aux = R1[2];
			break;
		case 3:
			count++;
			aux = R1[3];
			break;
		case 4:
			count++;
			aux = R1[4];
			break;
	}
	printf("index: %d\n", (int) index);
	return aux;
}

double Florence(
      double FREQ,
      double SI,
      double GAMMAI,
      double SPIN,
      double IREL,
      double DELTA2,
      double TAURM,
      double TAUVM,
      double TAUMM,
      double DPARAM,
      double EPARAM,
      double S4M,
      double GXM,
      double GYM,
      double GZM,
      double AXM,
      double AYM,
      double AZM,
      double DM,
      double DDM,
      double CONCM,
      double ACQ,
      double AMOLFRAM,
      double RKM,
      double ACONTM,
      double THETAM,
      double PHIM
)
{
	double PINP[27],R1[10];
      double aux;

	FREQ = log10(FREQ);

      PINP[0] = SI;
      PINP[1] = GAMMAI;
      PINP[2] = SPIN;
      PINP[3] = IREL;
      PINP[4] = DELTA2; // TAUS0M
      PINP[5] = TAURM;
      PINP[6] = TAUVM;
      PINP[7] = TAUMM;
      PINP[8] = 2.0; // TAUDELTA // TO FORCE PINP[4] TO BE DELTA2
      PINP[9] = DPARAM;
      PINP[10] = EPARAM;
      PINP[11] = S4M;
      PINP[12] = GXM;
      PINP[13] = GYM;
      PINP[14] = GZM;
      PINP[15] = AXM;
      PINP[16] = AYM;
      PINP[17] = AZM;
      PINP[18] = DM;
      PINP[19] = DDM;
      PINP[20] = CONCM;
      PINP[21] = ACQ;
      PINP[22] = AMOLFRAM;
      PINP[23] = RKM;
      PINP[24] = ACONTM;
      PINP[25] = THETAM;
      PINP[26] = PHIM;

	florencef77_(PINP,&FREQ,R1);
      aux=R1[1]+R1[2]+R1[3];
      aux=1./(1./aux+TAUMM)*( (AMOLFRAM == 0.0) ? 1.0 : AMOLFRAM*1e-3/111);

      aux=aux/CONCM*0.001 + R1[4];
    // printf("%lg %lg %lg %lg %lg %lg aux=%lg\n",pow(10,FREQ),R1[0],R1[1],R1[2],R1[3],R1[4],aux);
	// printf("%lg %lg\n",FREQ,R1[0]);
 	return R1[0];	
}

double Florence4(
	  double index,
      double FREQ,
      double SI,
      double GAMMAI,
      double SPIN,
      double IREL,
      double DELTA2,
      double TAURM,
      double TAUVM,
      double TAUMM,
      double DPARAM,
      double EPARAM,
      double S4M,
      double GXM,
      double GYM,
      double GZM,
      double AXM,
      double AYM,
      double AZM,
      double DM,
      double DDM,
      double CONCM,
      double ACQ,
      double AMOLFRAM,
      double RKM,
      double ACONTM,
      double THETAM,
      double PHIM
)
{
	static double R1[]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
   	static int count = 0;	
	double PINP[27];
    double aux;

	if (count < 1 || count > 3 ) {
		FREQ = log10(FREQ);

     	PINP[0] = SI;
    	PINP[1] = GAMMAI;
   		PINP[2] = SPIN;
      	PINP[3] = IREL;
      	PINP[4] = DELTA2; // TAUS0M
      	PINP[5] = TAURM;
      	PINP[6] = TAUVM;
      	PINP[7] = TAUMM;
      	PINP[8] = 2.0; // TAUDELTA // TO FORCE PINP[4] TO BE DELTA2
      	PINP[9] = DPARAM;
      	PINP[10] = EPARAM;
      	PINP[11] = S4M;
      	PINP[12] = GXM;
      	PINP[13] = GYM;
      	PINP[14] = GZM;
      	PINP[15] = AXM;
      	PINP[16] = AYM;
      	PINP[17] = AZM;
      	PINP[18] = DM;
      	PINP[19] = DDM;
      	PINP[20] = CONCM;
      	PINP[21] = ACQ;
      	PINP[22] = AMOLFRAM;
      	PINP[23] = RKM;
      	PINP[24] = ACONTM;
      	PINP[25] = THETAM;
      	PINP[26] = PHIM;

		florencef77_(PINP,&FREQ,R1);
  		aux=R1[1]+R1[2]+R1[3];
   		aux=1./(1./aux+TAUMM)*( (AMOLFRAM == 0.0) ? 1.0 : AMOLFRAM*1e-3/111);

   		aux=aux/CONCM*0.001 + R1[4];
    // printf("%lg %lg %lg %lg %lg %lg aux=%lg\n",pow(10,FREQ),R1[0],R1[1],R1[2],R1[3],R1[4],aux);
 		count = 0;
	//	printf("reset count: %d\n%lg %lg %lg %lg %lg %lg %lg\n",count, FREQ,R1[0],R1[1],R1[2],R1[3],R1[4],R1[5]);
//	printf("index: %d\n", (int) index);
	}
	switch ( (int) index ) {
		default:
			aux = R1[0];
			break;
		case 1:
			count++;
			aux = R1[1];
			break;
		case 2:
			count++;
			aux = R1[2];
			break;
		case 3:
			count++;
			aux = R1[3];
			break;
		case 4:
			count++;
			aux = R1[4];
			break;
	}
//	printf("index: %d\n", (int) index);
	return aux;
}

