#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void usage()
{
  fprintf(stderr,"usage: ccat file1 file2 file3 ...\n");
  fprintf(stderr,"       (max: 16 files)\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  FILE *f[16],*openf();
  char out[4096],s[128];
  int i,j,nf[16],nopen;

  if(argc==1 || argc > 17) usage();

  for(i=0;i<argc-1;i++) f[i] = openf(argv[i+1],"r");

  strcpy(out," ");

  while(1){
    for(i=0;i<argc-1;i++){
      if(fgets(s,128,f[i])){
	for(j=0;j<128;j++) if(s[j]=='\n') s[j]=32;
	strcat(out,s);
      }
      if(feof(f[i])) nf[i]=0;
      else nf[i]=1;
    }
    /*    printf("%s\n",out); */
    fprintf(stdout,"%s\n",out);
    for(j=0,nopen=0;j<argc-1;j++) nopen += nf[j];
    if(nopen==0) break;
    strcpy(out," ");
  }


  for(i=0;i<argc-1;i++) fclose(f[i]);
  exit(0);
}
