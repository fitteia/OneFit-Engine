/******************************************************************************/
/*            GCHI2_VN.C                  */
/******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "globals.h"
#include <string.h>
#include "gchi2_vn.h"
#include "funcsn.h"
#include "lfit1n.h"

#define PENA 1.0

double gchi2_vn(double *p, double *chisq)
// double    p[],chisq[];
{
  int   ma,*lista,mfit,ndata,i,k,mode,j,u;
   double   chisqt=0.0;
   double   **Data,*x,*y,*sig,*a;

   a     = dvector(1,Ma);

   if(p == NULL) mode=1;
   else mode = (int) p[1];

   for(i=1;i<=NT;i++) {
      x     = dvector(1,Ndata[i]);
      y     = dvector(1,Ndata[i]);
      sig   = dvector(1,Ndata[i]);
      Data  = VData[i-1];
      for(k=1;k<=Ma;k++) {
	if(!strcmp(AA_names[k-1][i-1],"AA")){
	  a[k] = AA[k][i];
	}
	else {
	  u=-1;
	  for(j=0;j<p[0];j++){		
	    if(!strcmp(ParNames[j],AA_names[k-1][i-1])){	
	      u=j;
	      /* printf("%s p[%d]=%f\n",AA_names[k-1][i-1],u,p[u]);*/
	      break;				
	    }					
	  }								
	  if(u<0) gfitn_error("GETPAR: ","one of your arguments is not matched by any parameter in the Parameter file"); 
	  else a[k]=p[u];
	}
	/* printf("gchi2_vn aa[%d]=%f\n",k,a[k]);*/
      }
      for(k=1;k<=Ndata[i];k++) {
	x[k]   = Data[k][1];
	y[k]   = Data[k][2];
	sig[k] = Data[k][3];
      }
      ma    = Ma;
      mfit  = Mfit;
      ndata = Ndata[i];
      lista = Lista;
	void (*f)(double, double *, int, double *, double ,int);
		f= funcsn;
      if(_T!=NULL) lfit1n(x,y,sig,ndata,a,ma,lista,mfit,
			 &chisq[i],f,p,_T[i],mode);
      else lfit1n(x,y,sig,ndata,a,ma,lista,mfit,
			 &chisq[i],funcsn,p,1.0,mode);
 
      for(k=1;k<=Ma;k++) {
/*         printf("a[%d]=%lg\n",k,a[k]); */
         AA[k][i] = a[k];
      }
      free_dvector(x,1,Ndata[i]);
      free_dvector(y,1,Ndata[i]);
      free_dvector(sig,1,Ndata[i]);
   }
   for(i=1;i<=NT;i++) chisqt += chisq[i];
   free_dvector(a,1,Ma);

   return chisqt;

}
/******************************************************************************/
/*                                 */
/******************************************************************************/

