/******************************************************************************/
/*            GCHI2_VN.C                  */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "gchi2out.h"
#include "fitutil.h"
#include "lfit1n.h"
#include "funcsn.h"

double gchi2out(double p[], int i, FILE *fitout)
// double    p[];
// int  i;  temperature i 
// FILE    *fitout;
{
   int   ma,*lista,mfit,ndata,j,k,mode;
   double   chisqt=0.0;
   double   **Data,*x,*y,*sig,*a;

   a     = dvector(1,Ma);

   if(p) mode = (int) p[1];
   else mode =1;

   x     = dvector(1,Ndata[i]);
   y     = dvector(1,Ndata[i]);
   sig   = dvector(1,Ndata[i]);
   Data  = VData[i-1];
   for(k=1;k<=Ma;k++) a[k] = AA[k][i]; 
   for(k=1;k<=Ndata[i];k++) {
     x[k]   = Data[k][1];
     y[k]   = Data[k][2];
     sig[k] = Data[k][3];
   }

   ma    = Ma;
   mfit  = Mfit;
   ndata = Ndata[i];
   
   lista = Lista;

   if(_T!=NULL) lfit1n(x,y,sig,ndata,a,ma,lista,mfit,
		      &chisqt,funcsn,p,_T[i],mode);
   else lfit1n(x,y,sig,ndata,a,ma,lista,mfit,
	       &chisqt,funcsn,p,1.0,mode);
   for(k=1;k<=Ma;k++) {
/*	                printf("a[%d]=%lg  ",k,a[k]);*/
       /*       printf("AA_NAMES=%s\n",AA_names[k-1][i-1]);*/
     if(!strcmp(AA_names[k-1][i-1],"AA")){
       /* printf("%s[%d]=%10.3e\n",AA_names[k-1][i-1],k,AA[k][i]); removed in webffit */
       fprintf(fitout,"%s[%d]=%10.3e\n",AA_names[k-1][i-1],k,AA[k][i]);
     }
     else {
       /* printf("%s=%10.3e\n",AA_names[k-1][i-1],AA[k][i]); removed in webffit */
       fprintf(fitout,"%s=%10.3e\n",AA_names[k-1][i-1],AA[k][i]);
     }
   }
   

   /* printf("\nCovar=\n");  removed in webffit */
   fprintf(fitout,"\nCovar=\n");
   
   for(k=1;k<=Ma;k++) {
     for(j=1;j<=Ma;j++){
       /* printf("0.0 "); removed in webffit */
       fprintf(fitout,"0.0 ");
     }
     /* printf("\n"); removed in webffit */
     fprintf(fitout,"\n");
   }
   /* printf("\n"); removed in webffit */
   fprintf(fitout,"\n");
   
   free_dvector(x,1,Ndata[i]);
   free_dvector(y,1,Ndata[i]);
   free_dvector(sig,1,Ndata[i]);
   
   free_dvector(a,1,Ma);
   
   return chisqt;

}
/******************************************************************************/
/*                                 */
/******************************************************************************/

