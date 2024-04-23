/******************************************************************************/
/*                  PP3D.C                  */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "globals.h"

#define P1M 1.5
#define P1m 0.5
#define P2M 1.5
#define P2m 0.5

static double sqrarg;
#define SQR(a) (sqrarg=(a),sqrarg*sqrarg)


void   pp3d(par)
double  par[];
{
   int   i,j,*ivector(),ntotpar,npar;
   double  *p[200],*dvector(),gchi2t(),lixo(),**dmatrix(),*parbak=0,p1,p2,z;
   double  *chisq,p1m,p1M,p2m,p2M,p1s,p2s;
   char    ppname[100];
   void    free_dvector(),free_ivector();
   FILE    *fpp,*openf(),*gnu3d;

   ntotpar = FPar+NT*Mfit;
   if(par) npar    = (int) par[0];
   else npar =0;

   chisq = dvector(1,NT);
   if(npar) parbak= dvector(0,npar-1);

   for(i=0;i<npar;i++) parbak[i]=par[i];

   for(i=0;i<FPar;i++) {
     p[i]=&par[ParFreeVector[i+1]];
/*     printf("%g %g\n",*p[i],par[ParFreeVector[i+1]]); */
   }
   for(i=0;i<NT;i++){
     for(j=0;j<Mfit;j++) {
       p[j+i+FPar]=&AA[Lista[j+1]][i+1];
/*       printf("%g %g\n",*p[j+i+FPar],AA[Lista[j+1]][i+1]); */
     }
        }
   for(i=0;i<ntotpar;i++) printf("p[%d]=%g\n",i,*p[i]);

   gnu3d = openf("scan3d.gnu","w");
   fprintf(gnu3d,"set parametric\n");
   fprintf(gnu3d,"set contour\n");
   fprintf(gnu3d,"set data style lines\n");

   for(i=0;i<ntotpar;i++) {
     for(j=i+1;j<ntotpar;j++){
       sprintf(ppname,"p%dp%d.3d",i,j);
       printf("%s\n",ppname);
       fpp = openf(ppname,"w");

       fprintf(gnu3d,"\nsplot \"%s\"\npause -1\n",ppname);
       fprintf(fpp,"# 3D plot of parameters %d and %d\n",i,j);

       p1 = *p[i];
       p2 = *p[j];

       if(p1<0.0) {p1m=P1M*p1; p1M=P1m*p1; p1s = (P1m-P1M)*p1/10.0;}
       else {p1m=P1m*p1; p1M=P1M*p1; p1s = (P1M-P1m)*p1/10.0;}
       if(p2<0.0) {p2m=P2M*p2; p2M=P2m*p2; p2s = (P2m-P2M)*p2/10.0;}
       else {p2m=P2m*p2; p2M=P2M*p2; p2s = (P2M-P2m)*p2/10.0;}


       for((*p[i])=p1m;(*p[i])<=p1M;(*p[i]) += p1s){
         for((*p[j])=p2m;(*p[j])<=p2M;(*p[j]) += p2s){
      z = gchi2t(par,chisq);
/*      printf(" %g %g %g\n",*p[i],*p[j],z);*/
      fprintf(fpp," %g %g %g\n",*p[i],*p[j],z);

         }
/*         printf("\n"); */
         fprintf(fpp,"\n");
       }
       *p[i]=p1;
       *p[j]=p2;

       fclose(fpp);
     }
   }
   for(i=0;i<npar;i++) par[i] = parbak[i];
   fclose(gnu3d);
   free_dvector(chisq,1,NT);
   if(npar) free_dvector(parbak,0,npar-1);
}
/******************************************************************************/
/*                                 */
/******************************************************************************/
double   gchi2t(par,chisq)
double    par[];
double chisq[];
{
   int   ma=0,i,k,j,mode;
   double   chisqt;
   double   **Data,*x,*y,*sig,*afunc,sum,t;
   double   *dvector();
   void   funcsn(),free_dvector();

   afunc = dvector(1,Ma);
   if(par) mode = (int) par[1];
   else mode =1;

   for(i=1;i<=NT;i++) {
      x     = dvector(1,Ndata[i]);
      y     = dvector(1,Ndata[i]);
      sig   = dvector(1,Ndata[i]);
      Data  = VData[i-1];
      chisq[i]=0.0;
      for(k=1;k<=Ndata[i];k++) {
         x[k]   = Data[k][1];
         y[k]   = Data[k][2];
         sig[k] = Data[k][3];

         t = _T[i];
         ma = Ma;
         funcsn(x[k],afunc,ma,par,t,mode);

         for (sum=0.0,j=1;j<=Ma;j++) sum += AA[j][i]*afunc[j];
         chisq[i] += SQR((y[k]-sum)/sig[k]);
      }

      free_dvector(x,1,Ndata[i]);
      free_dvector(y,1,Ndata[i]);
      free_dvector(sig,1,Ndata[i]);

   }
   chisqt=0.0;
   for(i=1;i<=NT;i++) chisqt += chisq[i];

   free_dvector(afunc,1,ma);

   return chisqt;

}
/******************************************************************************/
/*                                 */
/******************************************************************************/
double lixo(p)
double p[];
{
return 0.0;
}
#undef SQR

