#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "cop.h"
#include "fitutil.h"

/*
 ** The following lines will prevent `gcc' version 2.X
   from issuing an "unused variable" warning. **
static char *rcsid="$Id: cop.c,v 1.1 1997/11/07 12:12:06 root Exp $";
#if __GNUC__ == 4
#define USE(var) static void * use_##var = (&use_##var, (void *) &var)
USE(rcsid);
#endif
*/

#define COP(x,y) ( (*(x.fptr))(x,y) )

#define MAX_LINES 10000
#define MAX_COL  100

void usage()
{
  printf("usage: cop [-v] [-b?] [-e?] [-c?] [-nt[b]] [-GEF] ncol Ifname Ofname columns...\n");
  printf("           options:\n");
  printf("              -v  : screen report\n");
  printf("              -bx : begin line x\n");
  printf("              -ex : end line x\n");
  printf("              -cx : columns separator x\n");
  printf("              -ntb: no lines above (t) and/or below (b) those choosed\n");
  printf("              -GEF: G format, E format or F format. Default G format\n");
  printf("\n");
  printf("           ncol   : number of data columns in Ifname\n");
  printf("           Ifname : input file name\n");
  printf("                    . standard input\n");
  printf("           Ofname : output file name\n");
  printf("                    . standard output\n");
  printf("           columns:\n");
  printf("              constante\n");
  printf("               c#\n");
  printf("              -c#\n");
  printf("              c#[+-x/]c#\n");
  printf("              abs,ln,log,exp,sin,cos[c#]\n");
  printf("              c#[+-x/^]constante\n");
  printf("              constante[+-x/^]c#\n");
  printf("ATTENTION: When inside gnuplot use cop with \"-\" and \"^\" insted of - and ^.\n");
  exit(1);
}

int S_flag,G_flag,F_flag,p_flag,b_flag,e_flag,c_flag,top_flag,botom_flag,nb,ne;
char sep;

int main(int argc, char *argv[])
// int argc;
// char *argv[];
{
   maincop(argc,argv);
   exit(0);
}

void maincop(int argc, char *argv[])
// int argc;
// char *argv[];
{
  int i,j,k,nop,nc_out,nc,err=1;
  char c,ins[20];
  double x,*y;
  FILE *fin,*fout;
  Operacao col[MAX_COL];

  p_flag=b_flag=e_flag=c_flag=top_flag=botom_flag=nb=nop=0;
  S_flag=F_flag=G_flag=0;
  ne  = MAX_LINES;
  sep = ' ';

  if(argc <= 4) usage();

  switch(argc){
  case 5:
    break;
  case 6:
    options(argv[1]);
    break;
  case 7:
    options(argv[1]);
    options(argv[2]);
    break;
  case 8:
    options(argv[1]);
    options(argv[2]);
    options(argv[3]);
    break;
  case 9:
    options(argv[1]);
    options(argv[2]);
    options(argv[3]);
    options(argv[4]);
    break;
  case 10:
    options(argv[1]);
    options(argv[2]);
    options(argv[3]);
    options(argv[4]);
    options(argv[5]);
    break;
  default:
    options(argv[1]);
    options(argv[2]);
    options(argv[3]);
    options(argv[4]);
    options(argv[5]);
    options(argv[6]);
    break;
  }
/*
  printf("%d %d %d %d %d %d %d %d %d\n",
    p_flag,b_flag,e_flag,c_flag,top_flag,botom_flag,S_flag,F_flag,G_flag);
*/
  if(p_flag==1) nop++;
  if(b_flag==1) nop++;
  if(e_flag==1) nop++;
  if(c_flag==1) nop++;
  if(top_flag==1 || botom_flag==1) nop++;
  if(S_flag==1) nop++;
  if(F_flag==1) nop++;
  if(G_flag==1) nop++;

  nc_out = argc-nop-4;
/*
  printf("%d %d %d\n",nop,nc_out,ne);
  for(i=1;i<=argc;i++) printf("%s\n",argv[i]);
*/

  if(nc_out <= 0) usage();
  else if(argv[1+nop][0] < '0' || argv[1+nop][0] > '9') usage();
  else if( !strcmp(argv[2+nop],argv[3+nop]) && strcmp(argv[2+nop],".")) usage();
  else{
    nc  = atoi(argv[1+nop]);
    if(strcmp(argv[2+nop],".")) fin = openf(argv[2+nop],"r");
    else fin = stdin;
    if(strcmp(argv[3+nop],".")) fout= openf(argv[3+nop],"w");
    else fout = stdout;
    y   = dvector(1,nc);

    for(i=0;i<nc_out;i++) operacao(&col[i],argv[i+4+nop]);

/*    printf("%d %d %d %d %d\n",nc,nc_out,nb,ne,nop); */

    for(i=1;i<nb;i++){
      while( (c=getc(fin)) != '\n') {
	if(p_flag==1 && top_flag != 1) printf("%c",c);
	if(top_flag != 1) fprintf(fout,"%c",c);
      }
      if(p_flag==1 && top_flag != 1) printf("\n");
      if(top_flag != 1) fprintf(fout,"\n");
    }
    for(k=nb;k<=ne;k++){
      if(feof(fin)!=0) break;
      for(j=1;j<nc;j++) err = fscanf(fin,"%lf%c",&y[j],&c);
      err = fscanf(fin,"%lf",&y[nc]);
      if(feof(fin)!=0) break;
      for(i=0;i<nc_out-1;i++) {
	x = (*(col[i].fptr))(&col[i],y);
	if(p_flag==1) printf("(%lg)%lg%c",y[i+1],x,sep);
	if(S_flag==1) strcpy(ins,"%11.4le%c");
	else if(F_flag==1) strcpy(ins,"%lf%c");
	else if(G_flag==1) strcpy(ins,"%lg%c");
	else strcpy(ins,"%lg%c");
	fprintf(fout,ins,x,sep);
      }
      x = (*(col[i].fptr))(&col[nc_out-1],y);
      if(p_flag==1) printf("(%lg)%lg\n",y[nc_out],x);
      if(S_flag==1) fprintf(fout,"%11.4le\n",x);
      else if(F_flag==1) fprintf(fout,"%lf\n",x);
      else if(G_flag==1) fprintf(fout,"%lg\n",x);
      else fprintf(fout,"%lg\n",x);
    }
    c = getc(fin);
    while(1) {
      if(botom_flag==1) break;
      if(feof(fin)!=0) break;
      c=getc(fin);
      if(feof(fin)!=0) break;
      if(p_flag==1) printf("%c",c);
      fprintf(fout,"%c",c);
    }
    free_dvector(y,1,nc);
    fclose(fin);
    fclose(fout);
  }
  if (!err) printf("fscan() call error in cop.c, maincop()\n");
}

void options(char argu[])
// char argu[];
{
  if(!strncmp(argu,"-v",2)) p_flag=1;
  else if(!strncmp(argu,"-b",2)) {
    b_flag=1;
    nb    = atoi(&argu[2]);
  }
  else if(!strncmp(argu,"-e",2)){
    e_flag=1;
    ne    =atoi(&argu[2]);
  }
  else if(!strncmp(argu,"-c",2)) {
    c_flag=1;
    sep = argu[2];
  }
  else if(!strncmp(argu,"-nt",3)) {
    top_flag=1;
    if(argu[3]=='b') botom_flag=1;
  }
  else if(!strncmp(argu,"-nb",3)) {
    botom_flag=1;
    if(argu[3]=='t') top_flag=1;
  }
  else if(!strncmp(argu,"-E",2)) S_flag=1;
  else if(!strncmp(argu,"-F",2)) F_flag=1;
  else if(!strncmp(argu,"-G",2)) G_flag=1;
  else ;
/*
  else if(argu[0] == '-' && (argu[1] != 'v' || argu[1] != 'b' || argu[1] != 'c'|| argu[1] != 'e' || argu[1] != 'n')) usage();
*/
}

void operacao(Operacao *col,char *argu)
// Operacao *col;
// char *argu;
{
  int i,j,op_flag=0,ncar=0,err=1;
  char c;
  double x;

  for(i=0;i<strlen(argu);i++) if(argu[i] == 'c') ++ncar;
  for(i=0;i<strlen(argu);i++) {
    if(argu[i] == '^') { op_flag=1; break;}
    else if(argu[i] == '+') op_flag=2;
    else if(argu[i] == '-') op_flag=3;
    else if(argu[i] == 'x') op_flag=4;
    else if(argu[i] == '/') op_flag=5;
  }
  switch(ncar){
  case 0:
    (*col).t    = 1;
    (*col).fptr = id;
    (*col).c    = atof(argu);
    break;

  case 1:
    if(argu[0]=='c') {
      err = sscanf(argu,"c%d%c%lf",&i,&c,&x);
      (*col).i    = i;
      (*col).c    = x;
      switch(op_flag){
      case 0:
   (*col).t = 0;
   (*col).i = atoi(&argu[1]);
   (*col).fptr = id;
   break;

      case 1:
   (*col).fptr = Pow_;
   break;

      case 2:
   (*col).fptr = soma;
   (*col).t   = 1;
   break;

      case 3:
   (*col).fptr = dif;
   (*col).t    =1;
   break;

      case 4:
   (*col).fptr = mult;
   (*col).t    =  1;
   break;

      case 5:
   (*col).fptr = divi;
   (*col).t    =1;
   break;

      default:
   usage();
      }
    }

    else if(argu[0] == '-' && argu[1] == 'c') {
      (*col).i = atoi(&argu[2]);
      (*col).fptr = sim;
    }

    else if((argu[0]=='-' && argu[1] != 'c') || argu[0]=='.' || (argu[0]<='9' && argu[0] >='0')) {
      err = sscanf(argu,"%lf%cc%d",&x,&c,&i);
      (*col).i = i;
      (*col).c = x;
      (*col).t = 2;
      switch(op_flag){
      case 1:
   (*col).fptr = Pow_;
   break;

      case 2:
   (*col).fptr = soma;
   break;

      case 3:
   (*col).fptr = dif;
   break;

      case 4:
   (*col).fptr = mult;
   break;

      case 5:
   (*col).fptr = divi;
   break;

      default:
   usage();
      }
    }

    else {
      if(!strncmp(argu,"ln",2) || !strncmp(argu,"LN",2)){
   (*col).i    = atoi(&argu[4]);
   (*col).fptr = ln_;
      }
      else if(!strncmp(argu,"log",3) || !strncmp(argu,"LOG",3)){
   (*col).i    = atoi(&argu[5]);
   (*col).fptr = Log_;
      }
      else if(!strncmp(argu,"abs",3) || !strncmp(argu,"ABS",3)){
   (*col).i    = atoi(&argu[5]);
   (*col).fptr = Abs_;
      }
      else if(!strncmp(argu,"exp",3) || !strncmp(argu,"EXP",3)){
   (*col).i    = atoi(&argu[5]);
   (*col).fptr = Exp_;
      }
      else if(!strncmp(argu,"sin",3) || !strncmp(argu,"SIN",3)){
   (*col).i    = atoi(&argu[5]);
   (*col).fptr = Sin_;
      }
      else if(!strncmp(argu,"cos",3) || !strncmp(argu,"COS",3)){
   (*col).i    = atoi(&argu[5]);
   (*col).fptr = Cos_;
      }
      else nrerror("Operation not defined.");
    }
    break;

  case 2:
    err = sscanf(argu,"c%d%cc%d",&i,&c,&j);
    (*col).i = i;
    (*col).j = j;
    if(c=='+') (*col).fptr = soma;
    else if(c=='-') (*col).fptr = dif;
    else if(c=='x') (*col).fptr = mult;
    else if(c=='/') (*col).fptr = divi;
    else usage();
    break;

  default:
    usage();
  }
  if (!err) printf("fscan() call error in cop.c, maincop()\n");
}

double Abs_(Operacao *x, double y[])
// Operacao *x;
// double y[];
{
  return fabs(y[(*x).i]);
}

double soma(Operacao *x, double y[])
// Operacao *x;
// double y[];
{
  if((*x).t == 2) return (*x).c + y[(*x).i];
  else if((*x).t == 1) return y[(*x).i] + (*x).c;
  else return y[(*x).i]+y[(*x).j];
}

double dif(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  if((*x).t == 2) return (*x).c - y[(*x).i];
  else if((*x).t ==1) return y[(*x).i] - (*x).c;
  else return y[(*x).i]-y[(*x).j];
}

double mult( Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  if((*x).t == 2) return (*x).c * y[(*x).i];
  else if((*x).t ==1) return y[(*x).i] * (*x).c;
  else return y[(*x).i] * y[(*x).j];
}

double divi(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  if((*x).t == 2) return (*x).c / y[(*x).i];
  else if((*x).t==1) return y[(*x).i] / (*x).c;
  else return y[(*x).i] / y[(*x).j];
}

double Pow_(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  if((*x).t == 2) return pow( (*x).c, y[(*x).i]);
  else return pow(y[(*x).i],(*x).c);
}

double id(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  if((*x).t == 1) return (*x).c;
  else return y[(*x).i];
}

double sim(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  return -y[(*x).i];
}

double ln_(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  return log(y[(*x).i]);
}

double Log_(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  return log10(y[(*x).i]);
}

double Exp_(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  return exp(y[(*x).i]);
}

double Sin_(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  return sin(y[(*x).i]);
}

double Cos_(Operacao *x, double y[])
// Operacao *x;
// double *y;
{
  return cos(y[(*x).i]);
}

