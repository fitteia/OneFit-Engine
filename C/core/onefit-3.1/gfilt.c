#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "fitutil.h"
#include "gfilt.h"

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define MAX(a,b)  (((a)>(b)) ? (a) : (b))
#define MIN(a,b)  (((a)<(b)) ? (a) : (b))

void usage()
{
  printf("usage:  expfilt n npts file_in [file_out]\n");
  printf("        n data points y\n");
  printf("        npts point filter\n");
  printf("        if file_in is . then standard input is used\n");
  exit(0);
}

double EXPFILT(int i, int j, int npts)
// int i,j,npts;
{
  if(npts ==0) {
    if(i == j) return 1.0;
    else return 0.0;
  }
  else return exp(-(i-j)*(i-j)/(2.0*pow((double) npts,2.0)));
}

double *expfilt(double *y, int n, int npts)
// double *y;
// int n,npts;
{
  double *z,sum=0.0;
  int i,j,k;

  z = dvector(1,n);

  for(i=1;i<=n;i++){
    sum=0.0;
    for(j=MAX(1,i-5*npts),z[i]=0.0;j<=MIN(i+5*npts,n);j++) {
      k=j;
      z[i] += y[k]*EXPFILT(i,j,npts);
      sum += EXPFILT(i,j,npts);
    }
    z[i] /= sum;
/*    printf("z[%d]=%g\n",i,z[i]);*/
  }
  return z;
}


int main(int argc, int argv)
// int argc;
// char *argv[];
{
  FILE *fin,*fout;
  int i,n=1,npts=1;
  double *x,*y,*z;

  fin =stdin;
  fout=stdout;
  
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
    fscanf(fin,"%lf\n",&y[i]);
/*    printf("%d %g %g\n",i,x[i],y[i]);*/
  }
  
  z = expfilt(y,n,npts);

  for(i=1;i<=n;i++) fprintf(fout,"%g\n",z[i]);

  free_dvector(x,1,n);
  free_dvector(y,1,n);
  free_dvector(z,1,n);

  fclose(fin);
  fclose(fout);

  exit(0);
}
 
#undef SWAP
