#include <math.h>
#include <stdio.h>
#include "RotVold.h"
#include "t1_rot_vold.h"

#define pi 3.1415927  


double RotVold(double f,double delta, double Szz, double tauS,  double tauL, double k1, double p, double A0, double A1, double A2)
{
  return  T1_rot_vold(f,delta,Szz,tauS,tauL,k1,p,A0,A1,A2);
} 
