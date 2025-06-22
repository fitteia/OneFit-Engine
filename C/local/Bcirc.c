#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#include "Bcirc.h"
#include "integra.h"
#include "fitutil.h"
#include "GauuLegendreInt.h"

#define pi      3.141592653589793238


double Bcirc(double x, double z, double miu, double N, double R, double I, double NP, double flag) 
{
  	double af;
    Function X;

    clear_struct(&X,7);
    if(flag>1) { w_f_ptr(&X,_Bcirc_z); }
    else {     w_f_ptr(&X,_Bcirc_x); }
 
    X.par[0].low_v  = 0.0;
    X.par[0].high_v  = 2*pi;
    X.par[1].val  = x;
    X.par[2].val  = z;
    X.par[3].val  = miu;
    X.par[4].val  = N;
    X.par[5].val  = R;
    X.par[6].val  = I;

    af = GaussLegendreInt(&X,0,(int) NP);

    //           printf("%lf\n",af);
    return af;
}

double _Bcirc_x(Function *X)
{
  double phi,x,z,miu,N,R,I,af,sphi;

   phi = r_pval(X,0);
   x     = r_pval(X,1);
   z     = r_pval(X,2);
   miu = r_pval(X,3);
   N    = r_pval(X,4);
   R     = r_pval(X,5);
   I      = r_pval(X,6);
   sphi = sin(phi);

   af= miu/(4*pi)*N*R*I*z*sphi/pow(R*R+x*x+z*z-2*R*x*sphi,1.5);

  // printf("%lf %lf %lf %lf %lf %lf %lf %lf\n",phi,x,z,miu,N,R,I,af);
   return af;
}

double _Bcirc_z(Function *X)
{
  double phi,x,z,miu,N,R,I,af,sphi;

   phi = r_pval(X,0);
   x     = r_pval(X,1);
   z     = r_pval(X,2);
   miu = r_pval(X,3);
   N    = r_pval(X,4);
   R     = r_pval(X,5);
   I      = r_pval(X,6);
   sphi = sin(phi);

   af= miu/(4*pi)*N*R*I*(R-x*sphi)/pow(R*R+x*x+z*z-2*R*x*sphi,1.5);
   return af;
}
