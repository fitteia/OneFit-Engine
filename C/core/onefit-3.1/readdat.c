#include <stdio.h>
#include "globals.h"
#include <stdlib.h>

int read_data(char *fname)
{
  FILE *fin;
  int nlinhas,*nchar_linha,i;
  char **linhas;
  static int nfile=1;

  fin = openf(fname,"r");
 
  nchar_linha = Flc(fin,&nlinhas); /* vector with n of chars per line */

  linhas = lines(fin,nlinhas,nchar_linha);


  if(xy_flag) {
    if(swc(linhas[0])==1) {
	printf("gfitn warning: Data file format dosen't agree with the xy option\n"); 
	printf("gfitn run-time error: Impossible to continue...check data file and xyt option in setup file\n");
	exit(1);
    }
    _T = NULL;
    Ndata[nfile]   = nlinhas;
    VData[nfile-1] = dmatrix(1,nlinhas,1,3);
    for(i=1;i<=Ndata[nfile];i++) {
      if(swc(linhas[i-1])!=3) {
	printf("gfitn: warning: skipping data line %d -> %s.\nLine format dosen't match x y ey\n",i,linhas[i-1]); 
	Ndata[nfile]--;
      }
      else {
	sscanf(linhas[i-1],"%lf %lf %lf",
	       &VData[nfile-1][i][1],
	       &VData[nfile-1][i][2],
	       &VData[nfile-1][i][3]);
                printf("%g %g %g\n",
		VData[nfile-1][i][1],
		VData[nfile-1][i][2],
		VData[nfile-1][i][3]);

      }
    }
  }
  else if(xyt_flag) {
    if(swc(linhas[0])!=1) {
	printf("gfitn warning: Data file format dosen't agree with the xyt option\n"); 
	printf("gfitn run-time error: Impossible to continue...check data file and xyt option in setup file\n");
	exit(1);
    }
    nlinhas--;
    _T[nfile] = atof(linhas[0]);
    Ndata[nfile]   = nlinhas;
    VData[nfile-1] = dmatrix(1,nlinhas,1,3);

    for(i=1;i<=Ndata[nfile];i++){
      if(swc(linhas[i])!=3) {
	printf("gfitn: warning: skipping data line %d -> %s.\nLine format dosen't match x y ey\n",i,linhas[i]); 
	Ndata[nfile]--;
      }
      else {
	sscanf(linhas[i],"%lf %lf %lf",
	       &VData[nfile-1][i][1],
	       &VData[nfile-1][i][2],
	       &VData[nfile-1][i][3]);
	/*      printf("%g %g %g\n",
		VData[nfile-1][i][1],
		VData[nfile-1][i][2],
		VData[nfile-1][i][3]);
		*/
      }
    }
  }
  else if(xyz_flag) { 
    if(swc(linhas[0])==1) {
	printf("gfitn: warning: Data file format dosen't agree with the xyz option\n"); 
	printf("gfitn run-time error: Impossible to continue...check data file and xyz option in setup file\n");
	exit(1);
    }
    _T = NULL;
    Ndata[nfile]   = nlinhas;
    VData[nfile-1] = dmatrix(1,nlinhas,1,4);
    for(i=1;i<=Ndata[nfile];i++){
      if(swc(linhas[i])!=4) {
	printf("gfitn: warning: skipping data line %d -> %s.\nLine format dosen't match x y z ez\n",i,linhas[i]); 
	Ndata[nfile]--;
      }
      else {
	sscanf(linhas[i],"%lf %lf %lf %lf",
	       &VData[nfile-1][i][1],
	       &VData[nfile-1][i][2],
	       &VData[nfile-1][i][3],
	       &VData[nfile-1][i][4]);
	/*      printf("%g %g %g\n",
		VData[nfile-1][i][1],
		VData[nfile-1][i][2],
		VData[nfile-1][i][3],
		VData[nfile-1][i][4]);
		*/
      }
    }
  }
  fclose(fin);
  nfile++;

  printf("Gfitn warning: %d data points read from %s\n",Ndata[nfile-1],fname);

  return Ndata[nfile-1];
}

int print_data(file,nfile)
char *file;
int nfile;
{
  int nlinhas,i;
  FILE *f;

  f = openf(file,"w");

  if(xy_flag) {
    nlinhas = Ndata[nfile];
    for(i=1;i<=nlinhas;i++){
/*      printf("%g %g %g\n",
	     VData[nfile-1][i][1],
	     VData[nfile-1][i][2],
	     VData[nfile-1][i][3]);
*/
      fprintf(f,"%g %g %g\n",
	     VData[nfile-1][i][1],
	     VData[nfile-1][i][2],
	     VData[nfile-1][i][3]);
    }
  }
  else if(xyt_flag) {
    nlinhas = Ndata[nfile];
/*    printf("%g\n",_T[nfile]);*/
    for(i=1;i<=nlinhas;i++) {
/*      printf("%g %g %g\n",
	     VData[nfile-1][i][1],
	     VData[nfile-1][i][2],
	     VData[nfile-1][i][3]);
*/
      fprintf(f,"%g %g %g\n",
	     VData[nfile-1][i][1],
	     VData[nfile-1][i][2],
	     VData[nfile-1][i][3]);
    }
  }
  else if(xyz_flag) { 
    nlinhas = Ndata[nfile];
    for(i=1;i<=nlinhas;i++){
/*      printf("%g %g %g %g\n",
	     VData[nfile-1][i][1],
	     VData[nfile-1][i][2],
	     VData[nfile-1][i][3],
	     VData[nfile-1][i][4]);
*/
      fprintf(f,"%g %g %g %g\n",
	     VData[nfile-1][i][1],
	     VData[nfile-1][i][2],
	     VData[nfile-1][i][3],
	     VData[nfile-1][i][4]);
    }
  }
  fclose(f);
  return nfile;
}

