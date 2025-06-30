#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#include "integra.h"
#include "GaussLegendreInt.h"
#include "CompElliptic1.h"

#define	pi	3.141592653589793238
#define PI 3.1415926
#define	C	0.569504e-12
#define LIM	3.0e-11


double _elliptic1_(Function *X)
{
  double p1,p2;

   p1 = r_pval(X,0);
   p2 = r_pval(X,1);
    {

      /**** Your code starts here ****/
       double k,theta;
       k=p2;
       theta=p1;
       //       printf("k=%lf theta=%lg\n",k,theta);
       
       return 1.0/(sqrt(1-pow(k*sin(theta),2.0)));
      /**** Your code ends here****/

    }
}

double CompElliptic1(double K, double NP) 
{
  	double af;
    Function X;

    clear_struct(&X,2);
    w_f_ptr(&X,_elliptic1_);
    X.par[0].low_v  = 0.0;
    X.par[0].high_v  = (pi/2.0);
    X.par[1].val  = K;

    af = GaussLegendreInt(&X,0,(int) NP);

    //        printf("%lf\n",af);
    return af;
}


