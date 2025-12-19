#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "struct.h"
#include "userlib.h"
#include "mixed.h"
#include "R1spm-heuristic.h"

#define pi 3.1415926

#define gammae 1.76085963023e11
#define gamma 2.6752212881e8
#define kB 1.3806503e-23
#define hbar 1.05457148e-34
#define NA 6.0221415e23

/*****
*	Based on eq. 31 of Roch et al.paper J. Chem. Phys. 1999, 110 (11), 5403−5411
*	index: 0: total; 1: 1st term; 2: 2nd term; 3: 3th term
* 	f: frequency; Unit: Hz  
* 	T: temperature; Unit: Hz  
* 	C: concentration of magnetic particvles; Unit: mmol/L
*   mu: Magnetic moment os paramagnetic particles; Unit: A m^2
*   r: mean sqaure diffusion distance; (td=r^2/D); Unit: meter
*   P: relative paramagnertic contributions; Unit; none
*   D: self-diffusion coefficient; Unit: m^2/s
*   tN: Neel relaxation; Unit: second 
*****/

double __jhF(double w, double tD, double tN);
double __jhA(double w, double t);

double R1spmHeur(double index, double f, double T, double C, double mu, double r, double P, double D, double tN, double w0)
{
    double tD = r*r/D;
    double wI = 2*pi*f;
    double wS = wI*gammae/gamma;
    double x = mu*wI/(gamma*kB*T);
 
    double lgv = 1.0/tanh(x) - 1.0/x;
    double OmegaS =  (wS > w0) ? pow(pow(wS,0.25)-pow(w0,0.25), 4.0) : 0.0;

    double term1 = 7.0*lgv*P*__jhF(OmegaS,tD,tN)/x;
    double term2 = 7*(1-P)*lgv*__jhF(wI,tD,tN)/x;
    double term3 = 3*(1-lgv*lgv-2*lgv/x)*__jhF(wI,tD,tN) + 3*lgv*lgv*__jhA(wI,tD);
  
    double r1 = 32*pi/1350000.0 * 1e-14 * gamma*gamma * NA * C * mu*mu/(r*D);
   	
	switch ( (int) index ) {
		default:
			r1 = r1 * ( term1 + term2 + term3 );
			break;
		case 1:
			r1 = r1 * term1;
			break;
		case 2:
			r1 = r1 * term2;
			break;
		case 3:
			r1 = r1 * term3;
			break;
	}

    return r1;
} 

double __jhF(double w, double tD, double tN)
{
  double A,B,u,v,af;
  double im = w*tD;
  double re = tD/tN;

  A  = sqrt(sqrt(re*re+im*im));
  B  = (re>0.0) ? atan(im/re)/2.0 : pi/4.0;

  u  = A*A*A*cos(3*B)+9*A*cos(B)+4*re+9.0;
  v  = A*A*A*sin(3*B)+9*A*sin(B)+4*im;

  af = 9.0/4.0*(A*v*sin(B)+A*u*cos(B)+4*u)/(u*u+v*v);
  
  return af;
}

double __jhA(double w, double t)
{
    double u = sqrt(2*w*t);
    double u2 = u*u;
    double u3 = u2*u;
    return 81.0*( 8.0 + 5.0*u + u2 )/( 648*(1 + u) +324*u2 +108*u3 + 32*u2*u2 + 8*u2*u3 + u3*u3);
}


