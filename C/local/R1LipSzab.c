#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "struct.h"
#include "userlib.h"
#include "lor.h"
#include "R1LipSzab.h"

#define pi 3.1415926

#define gammae 1.76085963023e11
#define gamma 2.6752212881e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

/*****
*      f: frequency; Unit: Hz
*      q: number of coordinated water molecules; Unit: none
*      C: mili molar concentration of particles m; Unit: mmol/L
*      ms: molar mass of the solvent; Unit: kg/mol
*      S: electronic spin; Unit: none
*     tm: residence time of the water molecule in contact with the m center: Unit: s
*     tR: global rotation time of the m-q*water agregate; Unit: s 
*     tv: correlation time of the zero field splitting fluctuations; Unit; s
*      r: distance between spins S and I; Unit: m
* Delta2: Means sqaure fluctuations of the zero field spliting of the m species; Unit: s^{-2}
* Aohbar: A/hbar is the hyperfine coupling constant; Unit: rad/s
*    SLS: order parameter; Value between 0 and 1
*     tl: fast local reorientation time; Unit: s
*****/

double R1LipSzab(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh, double SLS, double tl)
{
  double af, R1m, R1msc, R1mdd, te, ws, wH, tc, tf, A;
  
  wH    = 2*pi*f;
  ws    = wH*gammae/gamma;
  te    = 1.0/( 1.0/25.0*Delta2*(4*S*(S+1)-3)*(lor(tv,ws)+4.0*lor(tv,2*ws)) );
  tc    = 1.0/(1.0/tm+1.0/tR+1.0/te);
  tf    = 1.0/(1.0/tc+1.0/tl);
  A     = 2.0/15.0*(1e-14)*gammae*gammae*gamma*gamma*hbar*hbar*pow(r,-6.0)*S*(S+1);
  R1mdd = A*(SLS*SLS*(3*lor(tc,wH) + 7*lor(tc,ws)) + (1-SLS*SLS)*(3*lor(tf,wH) + 7*lor(tf,ws)) );
  R1msc = 2.0/3.0*Aoh*Aoh*S*(S+1)*lor(te,ws);
  R1m   = R1mdd+R1msc;

  af= 1.0/1000.0*q*C*ms/rho/(1.0/R1m+tm);
  
  return af;
}
