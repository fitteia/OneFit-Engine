#include <math.h>
#include <stdio.h>

#define pi 3.1415926
#define Arot 6.408e11

/***
    ARot=3/4 K_D 
    K_D = 3/2 (mu0 g^2hbar/4pi)^2
***/


double RotWoessner(f, S,A0,A1,A2, Dx, Dz,delta)
double f, S, Dx,Dz,A0,A1,A2,delta;
{
  double JRotWoessner_(), fij();
  double J1,J2;
  double w,y;

  w=2*pi*f;
  J1 = 6*fij(1,0,delta)*JRotWoessner_(0,w,S,A0,A1,A2,Dx,Dz)+
    fij(1,1,delta)*JRotWoessner_(1,w,S,A0,A1,A2,Dx,Dz)+
    4*fij(1,2,delta)*JRotWoessner_(2,w,S,A0,A1,A2,Dx,Dz);
  J2 = 6*fij(2,0,delta)*JRotWoessner_(0,2*w,S,A0,A1,A2,Dx,Dz)+
    fij(2,1,delta)*JRotWoessner_(1,2*w,S,A0,A1,A2,Dx,Dz)+
    4*fij(2,2,delta)*JRotWoessner_(2,2*w,S,A0,A1,A2,Dx,Dz);

  y=Arot*(J1+J2);
  
  // printf("Relax_rate = %le", y);
  
  return y;
}

double JRotWoessner_(int mL, double w, double P2, double A0, double A1, double A2, double Dx, double Dz)
{
  double y;
  double c_(),itau0,itau1,itau2;

  itau0 = (6*Dx);
  itau1= (5*Dx+Dz);
  itau2= (2*Dx+4*Dz);

  y=A0*c_(mL,0,P2)*itau0/(w*w+itau0*itau0) +A1*c_(mL,1,P2)*itau1/(w*w+itau1*itau1) + A2*c_(mL,2,P2)*itau2/(w*w+itau2*itau2);

  //printf("A0=%lg A1=%lg A2=%lg itau0=%lg itau1=%lg itau2=%lg y=%lg\n",A0,A1,A2,tau0,tau1,tau2,c_(mL,0,P2));
  return 4.0/3.0*y;
}
