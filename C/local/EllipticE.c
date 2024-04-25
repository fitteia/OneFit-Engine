#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#define	pi	3.141592653589793238
#define PI 3.1415926
#define	C	0.569504e-12
#define LIM	3.0e-11


double EllipticE(double x, double K, double NP) 
{
  double _elliptice_(),GaussLegendreInt(),af;
    void w_f_ptr(),clear_struct();
    Function X;

    clear_struct(&X,2);
    w_f_ptr(&X,_elliptice_);
    X.par[0].low_v  = 0.0;
    X.par[0].high_v  = x;
    X.par[1].val  = K;

    af = GaussLegendreInt(&X,0,(int) NP);

    //        printf("%lf\n",af);
    return af;
}

double _elliptice_(Function *X)
{
  double r_pval(),p1,p2;

   p1 = r_pval(X,0);
   p2 = r_pval(X,1);
    {

      /**** Your code starts here ****/
       double k,theta;
       k=p2;
       theta=p1;
       //       printf("k=%lf theta=%lg\n",k,theta);
       
       return (sqrt(1-pow(k*sin(theta),2.0)));
      /**** Your code ends here****/

    }
}
