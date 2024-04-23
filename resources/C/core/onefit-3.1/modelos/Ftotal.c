#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "struct.h"


double   Ftotal(double x, double t, double *p)
{
  int   i,k,mode,j,u;
  double   *a,*afunc,sum;
   double   *dvector();
   void   funcsn(),free_dvector();

   a     = dvector(1,Ma);
   afunc= dvector(0,Ma);

   if(p == NULL) mode=1;
   else mode = (int) p[1];

   i=1;
/*   fprintf(stderr,"t=%lf T[2]=%lf\n",t,_T[2]); */
   while(t != _T[i]) i++;
/*   fprintf(stderr,"%d\n",i); */
   funcsn(x,afunc,Ma,p,t,mode);
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
/*     fprintf(stderr,"%lf\n",a[k]); */
   }
/*   for (sum=0.0,j=1;j<=Ma;j++) fprintf(stderr,"a[%d]=%lf,afunc[%d]=%f\n",j,a[j],j,afunc[j]);*/
   for (sum=0.0,j=1;j<=Ma;j++) sum += a[j]*afunc[j];
   free_dvector(a,1,Ma);
   free_dvector(afunc,0,Ma);
/* fprintf(stderr,"%lf\n",sum); */
   return sum;

}
