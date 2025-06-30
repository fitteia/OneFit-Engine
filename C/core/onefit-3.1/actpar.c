#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitutil.h"
#include "actpar.h"

void usage()
{
  printf("usage:  actpar out_file par_file [file_out]\n");
  printf("        if par_file is . then standard input is used\n");
  exit(0);
}

int main( int argc,char *argv[])
// int argc;
// char *argv[];
{
  FILE *fparin,*fout,*fparout;
  unsigned char c;
  int i,j,npar;
  char s[126],file[50],val[11];
  double *par;


  if(argc == 1) usage();
  else if (argc == 3) {
    if(argv[2][0]=='.') {
      fparin = stdin;
      fparout=stdout;
    }
    else {
      strcpy(s,"cp ");
      strcat(s,argv[2]);
      strcat(s," ");
      strcpy(file,argv[2]);
      strcat(file,"~");
      strcat(s,file);
      system(s);
      fparin  = openf(file,"r");
      fparout = openf(argv[2],"w");
    }
    fout = openf(argv[1],"r");
  }
  else if (argc == 4) {
    if(argv[2][0]=='.') fparin = stdin;
    else fparin  = openf(argv[2],"r");
    fparout = openf(argv[3],"w");
    fout = openf(argv[1],"r");
  }
  else usage();


  
  if( fgets(s,126,fout) == NULL) nrerror("fgets failure in actpar");
  if( fgets(s,126,fout) == NULL) nrerror("fgets failure in actpar");

  npar  = (int) ler_val(fout);
  par   = dvector(0,npar);
  par[0]= (double) npar;
  for(i=1;i<par[0];i++) par[i]=ler_val(fout);

  if( fgets(s,126,fparin) == NULL) nrerror("fgets failure in actpar");
  fprintf(fparout,"%s",s);
  for(i=0;i<npar;i++){
    if( fgets(s,126,fparin) == NULL) nrerror("fgets failure in actpar");
    switch(wc(s)){
    case 3:
      sscanf(s,"%s",val);
      fprintf(fparout,"%-10s",val);
      sscanf(&s[10],"%s",val);
      fprintf(fparout,"%-10s",val);
      fprintf(fparout,"%-10.3e\n",par[i]);
      break;
      
    case 4:
      sscanf(s,"%s",val);
      fprintf(fparout,"%-10s",val);
      sscanf(&s[10],"%s",val);
      fprintf(fparout,"%-10s",val);
      fprintf(fparout,"%-10.3e",par[i]);
      sscanf(&s[30],"%s",val);
      fprintf(fparout,"%-10s\n",val);
      break;
      
    case 5:
      sscanf(s,"%s",val);
      fprintf(fparout,"%-10s",val);
      sscanf(&s[10],"%s",val);
      fprintf(fparout,"%-10s",val);
      fprintf(fparout,"%-10.3e",par[i]);
      sscanf(&s[30],"%s",val);
      fprintf(fparout,"%-10s",val);
      sscanf(&s[40],"%s",val);
      fprintf(fparout,"%-10s\n",val);
      break;
      
    case 6:
      sscanf(s,"%s",val);
      fprintf(fparout,"%-10s",val);
      sscanf(&s[10],"%s",val);
      fprintf(fparout,"%-10s",val);
      fprintf(fparout,"%-10.3e",par[i]);
      sscanf(&s[30],"%s",val);
      fprintf(fparout,"%-10s",val);
      sscanf(&s[40],"%s",val);
      fprintf(fparout,"%-10s",val);
      sscanf(&s[50],"%s",val);
      fprintf(fparout,"%-10s\n",val);
      break;
      
    default:
      nrerror("case not defined in actpar");
    }
  }
  while(!feof(fparin)){
    if( fgets(s,126,fparin) == NULL) break;
    fprintf(fparout,"%s",s);
  }
  fclose(fparin);
  fclose(fout);
  fclose(fparout);

  exit(0);
}

#define IN  1
#define OUT 0

int wc(char *s)
// char *s;
{
  int nw,i,state;
  char c;

  nw = i = 0;
  state = OUT;

  while((c=s[i++]) != '\n'){
    if(c == ' ' || c == '\n' || c == '\t') state = OUT;
    else if(state == OUT){
      state = IN;
      ++nw;
    }
  }
  return nw;
}

double   ler_val(FILE *f)
// FILE   *f;
{
   double   x;
   char   c;
   while( (c=fgetc(f)) != '=');
   fscanf(f,"%lf",&x);
   return x;
}




