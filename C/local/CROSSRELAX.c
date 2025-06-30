#include <math.h>
#include <stdio.h>
#include "CROSSRELAX.h"

#define pi 3.14159265359

double CROSSRELAX(double f, double a, double tau, double f0)
{
  double	af;

  af= a*tau/((1+pow((f-f0)*tau,2.0)));
 
  return af;
}

double CROSSR1(double f, double a, double tau, double f0)
{
  double	af;

  af= a*( tau/(1+pow(2*pi*(f-f0)*tau,2.0)) + tau/(1+pow(2*pi*(f+f0)*tau,2.0)));
 
  return af;
}

double R1CRsp1(double f, double Cnh, double tq, double fq1, double fq2, double Theta, double Phi)
{
  double	af,c1,c2;

  c1 = 1.0/3.0+pow(sin(Theta)*cos(Phi),2.0);
  c2 = 1.0/3.0+pow(cos(Phi),2.0);
  
  af= Cnh*(
	     c1*( tq/(1+pow(2*pi*(f-fq1)*tq,2.0)) + tq/(1+pow(2*pi*(f+fq1)*tq,2.0)) )
	   + c1*( tq/(1+pow(2*pi*(f-fq2)*tq,2.0)) + tq/(1+pow(2*pi*(f+fq2)*tq,2.0)) )
	   + c2*( tq/(1+pow(2*pi*(f-(fq1-fq2))*tq,2.0)) + tq/(1+pow(2*pi*(f+(fq1-fq2))*tq,2.0)) )
    );
 
  return af;
}


































 /** XGFITN ADITIONAL CODE END **/
