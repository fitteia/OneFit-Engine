#include <stdio.h>
#include <math.h>
#include <string.h>
#include "globals.h"

/**********************************************************************/
/*                                                                    */
/**********************************************************************/
void ParFreeF(char filePar[])
// char filePar[];
{
   char   s[100]="",c;
   double pn,pv;
   int    i,j,nd,fnd,*status=0;
   int    npar,*pParfree=0;
   int    err;
   
   FILE   *fpar;

   fpar = openf(filePar,"r");
   new_line(fpar,1);
   err = fscanf(fpar,"%lf%s%lf",&pn,s,&pv);
   new_line(fpar,1);
/*
   printf("%11.3le %-11s %11.3le\n",pn,s,pv);
   fprintf(fileLog,"%11.3le %-11s %11.3le",pn,s,pv);
*/

   npar = (int) pv;
   if(npar>0) status  = ivector(0,npar-1);
   status[0]  = 0;

   for(i=1;i<npar;i++) {
     for(j=0;j<=19;j++) err = fscanf(fpar,"%c",&c);
     err = fscanf(fpar,"%lf",&pv);
      nd=fnd=0;
      for(j=0;j<100;j++) s[j]=0;
      j=99;
      do{
         c=fgetc(fpar);
         s[j]=c;
         if(c=='\n') {
            s[j]=0;
           break;
         } else if(c!=' ' && fnd==0) {
            nd += 1;
            fnd = 1;
         } else if(c==' ' && fnd==1) fnd=0;
      } while (c!='\n');

      switch(nd){
         case 0:
            status[i]=0;
            break;

         case 1:
            status[i]=1;
            break;

         case 2:
            status[i]=1;
            break;

         default:
            status[i]=1;
            break;
      }

 }
   while(!feof(fpar)) {
      err = fscanf(fpar,"%s",s);
      if(!strcmp(s,"fix")) {
         err = fscanf(fpar,"%lf",&pn);
         status[(int)pn-1]=0;
      }
   }
   for(FPar=0,i=0;i<npar;i++) if(status[i]==1) FPar += 1;
   if(FPar>0) pParfree = ivector(1,FPar);
   for(j=1,i=0;i<npar;i++) {
      if(status[i]==1) pParfree[j++]=i;
/*
      printf("parametro %d --> %d\n",i,status[i]);
      fprintf(fileLog,"parametro %d --> %d\n",i,status[i]);
*/
   }
/*
   printf("%FPar = %d\n",FPar);
*/
   if(npar>0) free_ivector(status,0,npar-1);
   fclose(fpar);
   ParFreeVector=pParfree;
   if (err<=0) printf("scanf read error in loadpar.c, ParFreeF():%d\n",err);

}
/**********************************************************************/
/*                                                                    */
/**********************************************************************/
char **ReadParNames(char filepar[])
// char filepar[];
{
  char **cmatrix(),**m;
  int i,npar;
  double x1,x2;
  char s[125]="",s1[11]="";
  void new_line();
  FILE *fin,*openf();
  int err=1;
  char *cerr=s;
  
  fin = openf(filepar,"r");

  new_line(fin,1);
  cerr = fgets(s,124,fin);
  err = sscanf(s,"%lf%s%lf",&x1,s1,&x2);

  npar = (int) x2;
  m = cmatrix(0,npar,0,11);

  strcpy(m[0],s1);
  for(i=1;i<npar;i++){
    cerr = fgets(s,124,fin);
    err = sscanf(s,"%lf%s",&x1,m[i]);
  }
  fclose(fin);
  if (err<=0) printf("scanf read error:%d\n",err);
  if (cerr==NULL) printf("fgets read error in ReadParNames\n");
  return m;
}
/**********************************************************************/
/*                                                                    */
/**********************************************************************/

