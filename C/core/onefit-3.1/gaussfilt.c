#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "fitutil.h"

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define MAX(a,b)  (((a)>(b)) ? (a) : (b))
#define MIN(a,b)  (((a)<(b)) ? (a) : (b))

void usage()
{
  printf("usage:  expfilt n npts file_in [file_out]\n");
  printf("        n data points (x,y)\n");
  printf("        npts point filter\n");
  printf("        if file_in is . then standard input is used\n");
  exit(0);
}
double EXPFILT(i,j,npts)
int i,j,npts;
{
  if(npts ==0) {
    if(i == j) return 1.0;
    else return 0.0;
  }
  else return exp(-(i-j)*(i-j)/((double) npts));
}
 
double *expfilt(y,n,npts)
double *y;
int n,npts;
{
  double *z,sum=0.0;
  int i,j;

  z = dvector(1,n);

  for(i=1;i<=n;i++){
    sum=0.0;
    for(j=MAX(1,i-5*npts),z[i]=0.0;j<=MIN(i+5*npts,n);j++) {
      z[i] += y[j]*EXPFILT(i,j,npts);
      sum += EXPFILT(i,j,npts);
    }
    z[i] /= sum;
/*    printf("z[%d]=%g\n",i,z[i]);*/
  }
  return z;
}

int main(int argc, char *argv[])
// int argc;
// char *argv[];
{
  FILE *openf(),*fin,*fout;
  char c;
  int i,n,npts;
  double *x,*y,*z;

  if (argc == 4) {
    n    = atoi(argv[1]);
    npts = atoi(argv[2]);
    if(argv[3][0]=='.') fin = stdin;
    else fin  = openf(argv[3],"r");
    fout = stdout;
  }
  else if (argc == 5) {
    n    = atoi(argv[1]);
    npts = atoi(argv[2]);
    if(argv[3][0]=='.') fin = stdin;
    else fin  = openf(argv[3],"r");
    fout = openf(argv[4],"w");
  }
  else usage();
  
  x = dvector(1,n);
  y = dvector(1,n);

  for(i=1;i<=n;i++) {
    fscanf(fin,"%lf%c%lf\n",&x[i],&c,&y[i]);
/*    printf("%d %g %g\n",i,x[i],y[i]);*/
  }
  
  z = expfilt(y,n,npts);

  for(i=1;i<=n;i++) fprintf(fout,"%g%c%g\n",x[i],c,z[i]);

  free_dvector(x,1,n);
  free_dvector(y,1,n);
  free_dvector(z,1,n);

  fclose(fin);
  fclose(fout);

  exit(0);
}
#undef SWAP
