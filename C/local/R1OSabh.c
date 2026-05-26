#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "struct.h"
#include "R1OSabh.h"
#include "R1OSabhf.h"

#define pi 3.1415926

#define gammae 1.76085963023e11
#define gamma 2.6752212881e8
#define k 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

/*****
*      f: frequency; Unite: Hz
*      C: mili molar concentration of particles m; Unit: mmol/L
*      S: electronic spin; Unit: none
*      a: distance of closet approach betwee outer-shere molecules and m paticles; Unit: m 
*      D: self-diffusion coefficient; Unit; m^2/s
*     tv: correlation time of the zero field splitting fluctuations; Unit; s
* Delta2: Means sqaure fluctuation sof the zero field spliting of the m species; Unit: s^{-2}
*****/

double R1OSabh(double f, double C, double S, double a, double D, double tv, double Delta2, double fn)
{
   return fn*R1OSabhf(f, C, S, a, D, tv, Delta2);
}

