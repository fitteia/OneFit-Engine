#include <stdio.h>
#include <math.h>
#include "globals.h"
#include "struct.h"

#define pi 3.1415926
#define k 1.380649e-23
#define hbar 1.054571817e-34
#define MAX 5.0
#define NA 6.022e23

/*** after the paper 

1H relaxation enhancement induced by nanoparticles in solutions: influence of magnetic properties and diffusion, D. Kruk, et al. 
The Journal of Chemical Physics 150, 184306 (2019); 
https://doi.org/10.1063/1.5082177

C is the concentration in  micro Molar/dm3

N=C*NA/1e3 number or particle per cubic meter

nmpR1HSc()  is R1C    eq 6
nmpR1Hszn() is R1F(n) eq 9 
nmpR1Hsze() is R1F(e) eq 11

****/




double mnpR1HSc(double f, double T, double gH, double gS, double C, double S, double d, double D, double NP)
{
  double w, wS, A, B, Sc, tau, Cdd2,result;
  double _Bs(), _jMP();

  tau=d*d/D;
  w=2*pi*f;
  wS=gS/gH*w;
  Sc=S*_Bs(wS, S, T);

  Cdd2=1e-14*gH*gH*gS*gS*hbar*hbar;
  
  result=432.0/5.0*Cdd2*C*NA/1e3*tau/(d*d*d)*Sc*Sc*_jMP(w*tau,0.0,NP);
  return result;
} 

double mnpR1Hszn(double f, double T, double gH, double gS, double C, double S, double d, double D, double T1e, double NP)
{
  double w, wS, A, B, Sc,Sz2, tau, Cdd2,result;
  double _Bs(), _jMP();

  tau=d*d/D;
  w=2*pi*f;
  wS=gS/gH*w;
  Sc=S*_Bs(wS, S, T);
  Sz2=S*(S+1)-Sc/tanh(hbar*wS/(2*k*T))-Sc*Sc;

  Cdd2=1e-14*gH*gH*gS*gS*hbar*hbar;

  result=432.0/5.0*Cdd2*C*NA/1e3*tau/(d*d*d)*Sz2*_jMP(w*tau,tau/T1e,NP);
  return result;
} 

double mnpR1Hsze(double f, double T, double gH, double gS, double C, double S, double d, double D, double T2e, double NP)
{
  double w, wS, A, B, Sc, tau, Cdd2,result;
  double _Bs(), _jMP();

  tau=d*d/D;
  w=2*pi*f;
  wS=gS/gH*w;
  Sc=S*_Bs(wS, S, T);

  Cdd2=1e-14*gH*gH*gS*gS*hbar*hbar;

  result=1008.0/5.0*Cdd2*C*NA/1e3*tau/(d*d*d)*Sc/tanh(hbar*wS/(2*k*T))*_jMP(wS*tau,tau/T2e,NP);
  return result;
} 
double _Bs(double wS, double S, double T)
{
  return 1.0/tanh(S*hbar*wS/(k*T))-1.0/(2*S*tanh(hbar*wS/(2*k*T)));
}

double _jMP(double a, double b, double NP)
{
  double result=0.0;
  int np;
  double sqgausn(),_j_int1MP(),_j_int2MP();
  void w_f_ptr(),clear_struct();
  Function X;

  clear_struct(&X,3);

  a=fabs(a); 
  b=fabs(b);

  X.par[1].val = fabs(a);
  X.par[2].val = fabs(b);
  np = (int) NP;
  
  if( a == 0.0 && b == 0.0 ) {
      X.par[0].low_v  = 0.0;
      X.par[0].high_v = MAX;
      w_f_ptr(&X,_j_int1MP);
      result = sqgausn(&X,0, np);
  }
  else if (a == 0.0 && b > 0.0) {
      w_f_ptr(&X,_j_int2MP);

      X.par[0].low_v  = 0.0;
      X.par[0].high_v = sqrt(b);
      result = sqgausn(&X,0, np);

      X.par[0].low_v  = sqrt(b);
      X.par[0].high_v = MAX;
      result += sqgausn(&X,0, np);
  }
  else if (a > 0.0) {
    w_f_ptr(&X,_j_int2MP);

    X.par[0].low_v  = 0.0;
    X.par[0].high_v = sqrt(a);
    result = sqgausn(&X,0, np);
    
    X.par[0].low_v  = sqrt(a);
    X.par[0].high_v = MAX;
    result += sqgausn(&X,0, np);
  }
  else { printf("ERROR in jG. Case not defined\n"); }
  
  return result;
}

double _j_int1MP(Function *X)
{
  double u,u2,u4,a,b;
  double r_pval();

  u = r_pval(X,0);
  a = r_pval(X,1);
  b = r_pval(X,2);

  u2=u*u;
  u4=u2*u2;
  
  return 1.0/(81+9*u2-2*u4+u4*u2);
}

double _j_int2MP(Function *X)
{
  double u,u2,u4,a,b,y;
  double r_pval();
  
  u = r_pval(X,0);
  a = r_pval(X,1);
  b = r_pval(X,2);
  
  u2=u*u;
  u4=u2*u2;
  
  return u*u/(81+9*u2-2*u4+u4*u2)*(u2+b)/((u2+b)*(u2+b)+a*a);
}







































