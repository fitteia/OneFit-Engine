/******************************************************************************/
/*                                    PPCOVAR.C                               */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "globals.h"

#define P1M 1.1
#define P1m 0.9
#define P2M 1.1
#define P2m 0.9

#define ROUNDOFF 1e-4
#define H        1e-1


void    ppcovar(fitout,par)
FILE    *fitout;
double  par[];
{
   int     i,j,*ivector(),ntotpar,npar,*indx;
   double  *p[200],*dvector(),gchi2t(),lixo(),**dmatrix(),*parbak=0,p1,p2;
   double  *chisq,dd2bak;
   double  ff00,ff10,ff_10,ff11,ff1_1,ff_11,ff_1_1,dd2,**dd,**bb,*col,d;
   double  h,hbak;
   double  roff;
   void    ludcmp(),lubksb();
   void    free_dvector(),free_ivector(),free_dmatrix();
   FILE    *openf();

   ntotpar = FPar+NT*Mfit;
   if(par == NULL) npar=0;
   else npar    = (int) par[0];

   chisq = dvector(1,NT);
   if(npar!=0) parbak= dvector(0,npar-1);
   dd = dmatrix(1,ntotpar,1,ntotpar);
   bb = dmatrix(1,ntotpar,1,ntotpar);
   col= dvector(1,ntotpar);
   indx = ivector(1,ntotpar);

   for(i=0;i<npar;i++) parbak[i]=par[i];

   for(i=0;i<FPar;i++) {
     p[i]=&par[ParFreeVector[i+1]];
/*        printf("%g %g\n",*p[i],par[ParFreeVector[i+1]]); */
   }
   for(i=0;i<NT;i++){
     for(j=0;j<Mfit;j++) {
       p[j+i+FPar]=&AA[Lista[j+1]][i+1];
/*          printf("%g %g\n",*p[j+i+FPar],AA[Lista[j+1]][i+1]); */
     }
   }
/*      for(i=0;i<ntotpar;i++) printf("p[%d]=%g\n",i,*p[i]); */

/*      gnu3d = openf("lixo.2dd","w"); */

   for(i=0;i<ntotpar;i++) {
     for(j=0;j<ntotpar;j++){
/*
       sprintf(ppname,"p%dp%d.3d",i,j);
       printf("%s\n",ppname);
*/
       p1 = *p[i];
       p2 = *p[j];

       h = 10.0 * H;
       dd2=0.0;
       do {
         dd2bak=dd2;
         hbak=h;
         h *= 0.1;
         *p[i]=p1;
         *p[j]=p2;
         ff00 = gchi2t(par,chisq);
         *p[i] = p1*(1 + h);
         ff10 = gchi2t(par,chisq);
/*
         *p[j] = p2*(1 + h);
         ff01 = gchi2t(par,chisq);
         *p[j] = p2*(1 - h);
         ff0_1 = gchi2t(par,chisq);
         *p[i] = p1*(1 + 2*h);
         ff20 = gchi2t(par,chisq);
         *p[i] = p1*(1 -2*h);
         ff_20 = gchi2t(par,chisq);
*/
         *p[i] = p1*(1 - h);
         ff_10 = gchi2t(par,chisq);
         *p[i] = p1*(1 + h);
         *p[j] = p2*(1 + h);
         ff11 = gchi2t(par,chisq);
         *p[i] = p1*(1 + h);
         *p[j] = p2*(1 - h);
         ff1_1 = gchi2t(par,chisq);
         *p[i] = p1*(1 - h);
         *p[j] = p2*(1 + h);
         ff_11 = gchi2t(par,chisq);
         *p[i] = p1*(1 - h);
         *p[j] = p2*(1 - h);
         ff_1_1 = gchi2t(par,chisq);
/*
         if(i==j) dd2 = -ff20+16*ff10-30*ff00+16*ff_10-ff_20;
         else     dd2 = ff10+ff_10+ff01+ff0_1-2*ff00-ff11-ff_1_1;
*/
         if(i==j) dd2 = ff10-2*ff00+ff_10;
         else     dd2 = ff11-ff1_1-ff_11+ff_1_1;

         printf("%d %d dd2=%g\n",i,j,dd2);
         roff=dd2/ff00;
         if(dd2*dd2bak < 0.0 || fabs(roff) < ROUNDOFF) break;

       } while(1);
/*       } while(dd2*dd2bak > 0.0 && fabs(roff) > ROUNDOFF);*/

       dd2=dd2bak;
       h = hbak;

       if(i==j) dd[i+1][j+1]=0.5*dd2/(p1*p1*h*h);
       else dd[i+1][j+1]=0.5*dd2/(4*p1*p2*h*h);

/*
       if(i==j) dd[i+1][j+1]=0.5*dd2/(12*p1*p1*h*h);
       else dd[i+1][j+1]= -0.5*dd2/(2*p1*p2*h*h);
*/
/*
       printf(" %g %g %g\n",*p[i],*p[j],dd[i+1][j+1]);
       fprintf(gnu3d," %g %g %g\n",*p[i],*p[j],dd[i+1][j+1]);
*/
       *p[i]=p1;
       *p[j]=p2;
     }
   }
   ludcmp(dd,ntotpar,indx,&d);
   for(j=1;j<=ntotpar;j++){
     for(i=1;i<=ntotpar;i++) col[i]=0.0;
     col[j]=1.0;
     lubksb(dd,ntotpar,indx,col);
     for(i=1;i<=ntotpar;i++) bb[i][j]=col[i];
   }
   printf("\nCovar total=\n");
   fprintf(fitout,"\nCovar total=\n");
   for(i=1;i<=ntotpar;i++) {
     for(j=1;j<=ntotpar;j++) {
       printf("%11.3e ",bb[i][j]);
       fprintf(fitout,"%11.3e ",bb[i][j]);
     }
     printf("\n");
     fprintf(fitout,"\n");
   }

   for(i=0;i<npar;i++) par[i] = parbak[i];
/*      fclose(gnu3d); */
   free_ivector(indx,1,ntotpar);
   free_dmatrix(dd,1,ntotpar,1,ntotpar);
   free_dmatrix(bb,1,ntotpar,1,ntotpar);
   free_dvector(col,1,ntotpar);
   free_dvector(chisq,1,NT);
   if(npar!=0) free_dvector(parbak,0,npar-1);
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
#undef H
#undef ROUNDOFF
#undef P1M
#undef P1m
#undef P2M
#undef P2m












