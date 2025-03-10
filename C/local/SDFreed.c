#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#include "globals.h"

#define pi 3.141592654

/***
    [f]=Hz
    [n]=spins/m^3
    [d]=m
    [tau]=s
    NP number of points in the integral calculation 
 ***/

double SDFreed(double f, double tau, double n, double d, double NP)
{
    double __JSDFreed_();
    double Kdd1=1.70888e-49;  /* 3/10*(mu0/(4*pi))^2*gamma^4*hbar^2 */
    double RSD;
   
    RSD=Kdd1*n*tau*pow(d,-3.0)*(__JSDFreed_(f,tau,NP)+4*__JSDFreed_(2*f,tau,NP));
    return RSD;
}

double SDFreedHF(double f1, double f2, double tau, double n, double d, double NP)
{
    double __JSDFreed();
    double Kdd1=1.70888e-49;  /* 3/10*(mu0/(4*pi))^2*gamma^4*hbar^2 */
    double RSD;
   
    RSD=Kdd1/3.0*n*tau*pow(d,-3.0)*(
             __JSDFreed(fabs(f1-f2),tau,NP) + 
           3*__JSDFreed(f1,tau,NP) + 
           6*__JSDFreed(f1+f2,tau,NP)
    );
    /* printf("%g %g %g\n", f1, f2, RSD); */
    return RSD;
}

double __JSDFreed_(double f, double tau, double NP)
{
    double __QSDFreed_();
    double af,w;
    w=2*pi*f;
    af=72*__QSDFreed_(w*tau,NP);
    return af;
}


double __QSDFreed_(double wt, double NP) 
{
    double __integral0_(),sqgausn(),a1,a2,af;
    double b,c,d,e,wtmax;
    int np;
    void w_f_ptr(),clear_struct();
    Function X;

    np=(int) NP;

         b = -7.72764e-01;
         c = 2.01190e+00;
         d = 6.33007e+00;
         e = 5.00000e-01;
         wtmax=pow(d/(1+pow(wt/c,b)),e);

    clear_struct(&X,2);
    w_f_ptr(&X,__integral0_);
    X.par[0].low_v  = 0.0;
    X.par[0].high_v  = wtmax;
    X.par[1].val  = wt;

    a1 = sqgausn(&X,0,np);

    X.par[0].low_v  = wtmax;
    if(wt<=1) { X.par[0].high_v  = 7.5; }
    else if(wt<=10 && wt>1) { X.par[0].high_v  = 15; }
    else if(wt<=100 && wt>10) { X.par[0].high_v  = 30; }
    else { X.par[0].high_v  = 50; }
    X.par[1].val  = wt;

    a2 = sqgausn(&X,0,np);

    //printf("wt=%lf QSDFreed=%lf\n",wt,a1+a2);
    return (a1+a2);
}
double __integral0_(Function *X)
{
   double r_pval(),r_plow(),sqgausn(),af;
   double u,wt;

   u = r_pval(X,0);
   wt = r_pval(X,1);
    {

      /**** Your code starts here ****/
       double a,z,y1,y2;
       z=u*u;
       a=wt;
       y1=z/(81+9*z-2*z*z+z*z*z);
       y2=z/(z*z+a*a);
       return y2*y1;
       
      /**** Your code ends here****/

    }
}
