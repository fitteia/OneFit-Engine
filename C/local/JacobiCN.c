#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#include "JacobiCN.h"
#include "EllipticF.h"

#define	pi	3.141592653589793238
#define PI 3.1415926
#define N 8


double JacobiCN(double u, double k, double ni)
{
  double q,K,K1,af=0.0;
  int n;

  K = EllipticF(pi/2.0,k,ni);
  K1= EllipticF(pi/2.0,sqrt(1-k*k),ni);
  q = exp(-pi*K1/K);
  for(n=1;n<=N;n++) {
    af += pow(q,n-0.5)/(1+pow(q,2*n-1))*cos((n-0.5)*pi/K*u);
  }
  af *= 2*pi/k/K;
  return af;
}
