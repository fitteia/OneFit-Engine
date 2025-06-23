#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#include "fitutil.h"
#include "integra.h"
#include "GaussLegendreInt.h"

#define pi      3.141592653589793238

double _Bcoil_x(Function *X)
{
  double phi,x,z,miu,nz,nr,dz,R,I,af=0.0,sphi,i,j,zi,Rj;

   phi = r_pval(X,0);
   x     = r_pval(X,1);
   z     = r_pval(X,2);
   miu = r_pval(X,3);
   nz   = r_pval(X,4);
   nr    = r_pval(X,5);
   dz   = r_pval(X,6);
   R     = r_pval(X,7);
   I      = r_pval(X,8);
   sphi = sin(phi);

   for(i=1;i<=nz;i++){
        for(j=1;j<=nr;j++) {
	  zi = z+(i-0.5)*dz;
	  Rj = R+(j-0.5)*fabs(dz);
	  af += miu/(4*pi)*Rj*I*zi*sphi/pow(Rj*Rj+x*x+zi*zi-2*Rj*x*sphi,1.5);
        }
   }

  // printf("%lf %lf %lf %lf %lf %lf %lf %lf\n",phi,x,z,miu,N,R,I,af);
   return af;
}

double _Bcoil_z(Function *X)
{
  double phi,x,z,miu,nz,nr,dz,R,I,af=0.0,sphi,i,j,zi,Rj;

    phi = r_pval(X,0);
   x     = r_pval(X,1);
   z     = r_pval(X,2);
   miu = r_pval(X,3);
   nz   = r_pval(X,4);
   nr    = r_pval(X,5);
   dz   = r_pval(X,6);
   R     = r_pval(X,7);
   I      = r_pval(X,8);
  sphi = sin(phi);

   for(i=1;i<=nz;i++){
        for(j=1;j<=nr;j++) {
	  zi = z+(i-0.5)*dz;
	  Rj = R+(j+0.5)*fabs(dz);
	  af += miu/(4*pi)*Rj*I*(Rj-x*sphi)/pow(Rj*Rj+x*x+zi*zi-2*Rj*x*sphi,1.5);
        }
   }
   return af;
}

double Bcoil(double x, double z, double miu, double nz, double nr, double dz, double R, double I, double NP, double flag) 
{
  double af;
    Function X;

    clear_struct(&X,9);
    if(flag>1) { w_f_ptr(&X,_Bcoil_z); }
    else {     w_f_ptr(&X,_Bcoil_x); }
 
    X.par[0].low_v  = 0.0;
    X.par[0].high_v  = 2*pi;
    X.par[1].val  = x;
    X.par[2].val  = z;
    X.par[3].val  = miu;
    X.par[4].val  = nz;
    X.par[5].val  = nr;
    X.par[6].val  = dz;
    X.par[7].val  = R;
    X.par[8].val  = I;


    af = GaussLegendreInt(&X,0,(int) NP);

    //           printf("%lf\n",af);
    return af;
}

