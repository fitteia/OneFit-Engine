#include <stdio.h>
#include <string.h>
#include <math.h>
#include "struct.h"
#include <stdlib.h>

int main (int argc, char **argv)
{
  int     i=0,n=0,j=0,N=0;
  double	t,t1,a,x;
  char    file_in[40],c;
  char    file_out[40];
  double *dvector(),*data,*re,*im, delta,fstep,f,atof();

	void	drealfft(),expri(),exit();
	FILE	*fp,*openf(),*fout;

	if(argc < 2) {
	  fout=openf("/tmp/__fft.txt__","w");
	  while( (c = getc(stdin)) != EOF){
	    fprintf(fout,"%c",c);
	  }
	  fclose(fout);
	  fp  = openf("/tmp/__fft.txt__","r");
	  N=0;
	  t=t1=0;
	  while(!feof(fp)){
	    N++;
	    fscanf(fp,"%lf %lf\n",&t,&a);
	    delta=t-t1;
	    t1=t;
	  }
	  fstep = 1.0/(N*delta);
	  rewind(fp);

	}
	else if(argc == 4){
	  N    = atoi(argv[3]);
	  fp  = openf(argv[1],"r");
	  delta = atof(argv[2]);
	  fstep = 1.0/(N*delta);
	}
	else if(argc == 3){
	  fp  = openf(argv[1],"r");
	  delta = atof(argv[2]);
	  N=0;
	  while(!feof(fp)){
	    N++;
	    fscanf(fp,"%lf %lf\n",&t,&a);
	  }
	  fstep = 1.0/(N*delta);
	  rewind(fp);
	}
	else {
	  if(!strcmp(argv[1],"-h")) {
	    printf("usage: fft < filename\n       cat filename | fft\n       fft filename\n       fft filename delta\n       fft filename delta 2^n\n"); 
	    exit(1);
	  }
	  fp  = openf(argv[1],"r");
	  N=0;
	  t=t1=0;
	  while(!feof(fp)){
	    N++;
	    fscanf(fp,"%lf %lf\n",&t,&a);
	    delta=t-t1;
	    t1=t;
	  }
	  fstep = 1.0/(N*delta);
	  rewind(fp);
	}
	n=N;
        x=log(N)/log(2);
	if(x>floor(x)) {
	  n=pow(2.0,floor(x)+1);
	}
	/*	fprintf(stderr,"%d %d\n",n,N); */
	data = dvector(1,n);
	re = dvector(0,n/2);
	im = dvector(0,n/2);
	for(i=1;i <= n;i++) {
	  if(!feof(fp)) fscanf(fp,"%lf %lf\n",&t,&a);
	  else a = 0.0;
	  if(i==1 || i==n) {
	    data[i]=0.5*a;
	  } else {
	    data[i]=a;
	  }
/*
		fprintf(out,"%lg %lg %lg\n",t,a1,a2);	      	      
		printf("%lg %lg %lg\n",t,a1,a2);	      	      
*/
	}
	fstep = 1.0/(n*delta); /* Bug descoberto em 20/3/2018 conjuntamente por Pedro e Luis NG.*/
	drealfft(data,n/2,1);
	expri(data,re,im,n/2);
	/*	fout = fopen("/tmp/lixo.txt","w"); */
	for(i=-n/2+1;i <= 0;i++) {
	  f=(double) i*fstep;
	  re[-i] = re[-i]*delta;
	  im[-i] = im[-i]*delta;
	  printf("%lg %lg %lg\n",f,re[-i],-im[-i]);
	  /*	  fprintf(fout,"%lg %lg %lg\n",f,re[-i],-im[-i]);*/
	}
	for(i=1;i < n/2;i++) {
	  f=(double) i*fstep;
	  printf("%lg %lg %lg\n",f,re[i],im[i]);
	  /*	  fprintf(fout,"%lg %lg %lg\n",f,re[i],im[i]);*/
	}
	
	fclose(fp);
	if(argc<2) system("rm /tmp/__fft.txt__");
	/*	fclose(fout);*/
	/*	system("/sw/bin/xmgrace -nxy /tmp/lixo.txt"); */
	exit(0);

}

