#include <math.h>
#include <stdio.h>
#include "RNordio.h"
#include "fij.h"

#define pi 3.1415926
#define Arot 6.408e11

/***
    ARot=3/4 K_D 
    K_D = 3/2 (mu0 g^2hbar/4pi)^2
***/


double RNordio( double f,  double S0, double A0, double A1, double A2,  double Dz,  double Dx)
{
  double y;
  double J1;
  double J2;
  double w;

  w=2*pi*f;
  J1=JRotNordio_(1,w,S0,A0,A1,A2,Dz,Dx);
  J2=JRotNordio_(2,w,S0,A0,A1,A2,Dz,Dx);
  
  y=Arot*(J1+J2);
  
  // printf("Relax_rate = %le", y);
  
  return y;
}

double RNordio_ang( double f,  double S0, double A0, double A1, double A2,  double Dz,  double Dx, double delta)
{
  double y;
  double J1;
  double J2;
  double w;
  double fij();

  w=2*pi*f;
  J1 = fij(1,0,delta)*JRotNordio_2(0,w,S0,A0,A1,A2,Dz,Dx)
     + fij(1,1,delta)*JRotNordio_2(1,w,S0,A0,A1,A2,Dz,Dx)
     + fij(1,2,delta)*JRotNordio_2(2,w,S0,A0,A1,A2,Dz,Dx);
  J2 = fij(2,0,delta)*JRotNordio_2(0,2*w,S0,A0,A1,A2,Dz,Dx)
     + fij(2,1,delta)*JRotNordio_2(1,2*w,S0,A0,A1,A2,Dz,Dx)
     + fij(2,2,delta)*JRotNordio_2(2,2*w,S0,A0,A1,A2,Dz,Dx);
  
  y=Arot*(J1+J2);
  
  // printf("Relax_rate = %le", y);
  
  return y;
}

double RNordio_poli( double f,  double S0, double A0, double A1, double A2,  double Dz,  double Dx)
{
  double y;
  double J1;
  double J2;
  double w;

  w=2*pi*f;
  J1 =  1.0/30.0*JRotNordio_2(0,w,S0,A0,A1,A2,Dz,Dx)
     + 12.0/30.0*JRotNordio_2(1,w,S0,A0,A1,A2,Dz,Dx)
     +  3.0/30.0*JRotNordio_2(2,w,S0,A0,A1,A2,Dz,Dx);
  J2 =  4.0/30.0*JRotNordio_2(0,2*w,S0,A0,A1,A2,Dz,Dx)
     + 48.0/30.0*JRotNordio_2(1,2*w,S0,A0,A1,A2,Dz,Dx)
     + 12.0/30.0*JRotNordio_2(2,2*w,S0,A0,A1,A2,Dz,Dx);
  
  y=Arot*(J1+J2);
  
  // printf("Relax_rate = %le", y);
  
  return y;
}

/** eq. 7.61 Dong's Book 2nd ed **/
double JRotNordio_(int mL, double w, double P2, double A0, double A1, double A2, double Dz, double Dx)
{
  double y;
  double itau0,itau1,itau2;

  itau0=itau_(mL,0,Dz,Dx,P2);
  itau1=itau_(mL,1,Dz,Dx,P2);
  itau2=itau_(mL,2,Dz,Dx,P2);

  y=A0*c_(mL,0,P2)*itau0/(mL*mL*w*w+itau0*itau0) +A1*c_(mL,1,P2)*itau1/(mL*mL*w*w+itau1*itau1) + A2*c_(mL,2,P2)*itau2/(mL*mL*w*w+itau2*itau2);
  //printf("A0=%lg A1=%lg A2=%lg itau0=%lg itau1=%lg itau2=%lg y=%lg\n",A0,A1,A2,1.0/itau0,1.0/itau1,1.0/itau2,c_(mL,0,P2));
  return (mL>0) ? 4.0/3.0*mL*mL*y : 4.0/3.0*6*y;
}

/** eq. 7.61 Dong's Book 2nd ed com mLw substituido por w**/
double JRotNordio_2(int mL, double w, double P2, double A0, double A1, double A2, double Dz, double Dx)
{
  double y;
  double itau0,itau1,itau2;

  itau0=itau_(mL,0,Dz,Dx,P2);
  itau1=itau_(mL,1,Dz,Dx,P2);
  itau2=itau_(mL,2,Dz,Dx,P2);

  y=A0*c_(mL,0,P2)*itau0/(w*w+itau0*itau0) +A1*c_(mL,1,P2)*itau1/(w*w+itau1*itau1) + A2*c_(mL,2,P2)*itau2/(w*w+itau2*itau2);
  //printf("A0=%lg A1=%lg A2=%lg itau0=%lg itau1=%lg itau2=%lg y=%lg\n",A0,A1,A2,1.0/itau0,1.0/itau1,1.0/itau2,c_(mL,0,P2));
  return (mL>0) ? 4.0/3.0*mL*mL*y : 4.0/3.0*6*y;
}

double itau_(int mL,int mM, double Dz, double Dx,double P2)
{
   double y;

   y=Dx*(1.0/beta_(mL,mM,P2)+(Dz/Dx-1)*mM*mM);
   // printf("itau(%d,%d)=%lg\n",mL,mM,y);
  return y;
}

/**** beta(mL,mM) from A131 Supporting Info or Fig. 7.1 Dong's Book *****/
/**
   0-> 0,0 
   1-> 0,1 
   2-> 0,2 
   3-> 1,1 
   4-> 1,2 
   5-> 2,2 
**/
double beta_(int mL, int mM, double P2)
{
double beta_ref[6][5]={
{0.1655,0.0835,-0.0390,-0.3791,0.1707}, 
{0.1655,0.0289,-0.0085,-0.2947,0.1194},
{0.1655,-0.0934,-0.1010,0.2105,-0.1793},
{0.1655,0.0005,-0.1367,0.0212,0.0489},
{0.1655,-0.0499,-0.0966,0.1209,-0.1391},
{0.1655,0.1243,-0.0399,0.0006,0.0}
};

double y=0;
int i,n;
   if(mL==0 && mM==0) n=0;
   else if (mL==0 && mM==1) n=1;
   else if (mL==0 && mM==2) n=2;
   else if (mL==1 && mM==1) n=3;
   else if (mL==1 && mM==2) n=4;
   else if (mL==2 && mM==2) n=5;
   else if (mL==1 && mM==0) n=1;
   else if (mL==2 && mM==0) n=2;
   else if (mL==2 && mM==1) n=4;
   else printf("mL and mM incompatible\n");

  for(i=0;i<5;i++){
    y=y+beta_ref[n][i]*pow(P2,i);
   // printf("%lg ",y);
  }
  // printf("\nbeta(%d,%d,%lg)=%lg\n",mL,mM,P2,y); 
  return y;
}

/**** c(mL,mM) instead of k(mL,mM), Dong's Book eq. 7.61 and 1988-JCP88pp1443 *****/
/**
   0-> 0,0
   1-> 0,1
   2-> 1,1
   3-> 1,2
   4-> 2,0
   5-> 2,2
**/
double c_(int mL, int mM, double P2)
{
double c_ref[6][5]={
{0.201,0.234,-0.250,-1.069,0.885}, 
{0.199,0.187,-0.546,0.790,-0.631},
{0.198,0.131,-0.106,0.269,0.0},
{0.200,-0.137,-0.092,0.137,-0.108},
{0.200,-0.29,0.133,-0.190,0.153},
{0.200,0.289,-0.005,0.015,0.0}
};

double y=0;
int i,n;

   if(mL==0 && mM==0) n=0;
   else if (mL==0 && mM==1) n=1;
   else if (mL==0 && mM==2) n=4;
   else if (mL==1 && mM==1) n=2;
   else if (mL==1 && mM==2) n=3;
   else if (mL==2 && mM==2) n=5;
   else if (mL==1 && mM==0) n=1;
   else if (mL==2 && mM==0) n=4;
   else if (mL==2 && mM==1) n=3;
   else printf("mL and mM incompatible\n");

  for(i=0;i<5;i++){
    y=y+c_ref[n][i]*pow(P2,i);
    // printf("%lg ",y);
  }
  // printf("\nc(%d,%d,%lg)=%lg\n",mL,mM,P2,y); 
  return y;
}
