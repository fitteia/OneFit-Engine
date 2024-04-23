
#include <math.h>
#include <stdio.h>

#define pi 3.1415926

#define gammae 1.76085963023e11
#define gamma 2.6752212881e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

/*****
*      f: frequency; Unite: Hz
*      q: number of coordinated water molecules; Unit: none
*      C: mili molar concentration of particles m; Unit: mmol/L
*     ms: molar mass of the solvent; Unit: kg/mol
*    rho: density of the solution; Unit: g/cm^3=kg/L
*      S: electronic spin; Unit: none
*     tm: residence time of the water molecule in contact with the m center: Unit: s
*     tR: global rotation time of the m-q*water agregate; Unit: s 
*     tv: correlation time of the zero field splitting fluctuations; Unit; s
*      r: distance between spins S and I; Unit: m
* Delta2: Means sqaure fluctuation sof the zero field spliting of the m species; Unit: s^{-2}
*    Aoh: A/hbar is the hyperfine coupling constant; Unit: rad/s
*****/

double R1ISsbm(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh)
{
  double x,wI,wS, Delta,af,R1m,R1mdd,R1msc,tc1,tc2,te2,it1s,it2s;
  double lor();
  
   wI     = 2*pi*f;
   wS    = wI*gammae/gamma;
  
   it1s  = 1.0/25.0*Delta2*(4*S*(S+1)-3)*(lor(tv,wS)+4.0*lor(tv,2*wS));
   it2s  = 1.0/50.0*Delta2*(4*S*(S+1)-3)*(3*tv+5.0*lor(tv,wS)+2.0*lor(tv,2*wS));

   tc1   = 1.0/(1.0/tm+1.0/tR+it1s);
   tc2   = 1.0/(1.0/tm+1.0/tR+it2s);
   te2   = 1.0/(1.0/tm+it2s);

   Delta = hbar*gammae*gamma/pow(r,3.0)*1e-7;

   R1mdd = 2./15.*Delta*Delta*S*(S+1)*(3*lor(tc1,wI) + 7*lor(tc2,wS));
   R1msc = 2.0/3.0*Aoh*Aoh*S*(S+1)*lor(te2,wS);
   R1m   = R1mdd+R1msc;

   af= 1.0/1000.0*q*C*ms/rho/(1.0/R1m+tm);
   
   return af;
}

