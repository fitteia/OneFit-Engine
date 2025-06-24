/*****************************************************************************/
/*             FITUTIL.C                                    */
/*****************************************************************************/
#ifndef MacOSX
     #include "malloc.h"
#endif
#include "stdio.h"
#include "math.h"
#include <stdlib.h>
#include <string.h>
#include "fitutil.h" 

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void nrerror(char error_text[])
{
   fprintf(stderr,"OneFit run-time error...\n");
   fprintf(stderr,"%s\n",error_text);
   fprintf(stderr,"...now exiting to system...\n");
   exit(1);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void gfitn_error(char error_text[],char option_msg[])
{
   fprintf(stderr,"OneFit run-time error...\n");
   fprintf(stderr,"%s %s\n",error_text,option_msg);
   fprintf(stderr,"...now exiting to system...\n");
   exit(1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
int *ivector(int nl, int nh)
{
   int *v;

   v=(int *)malloc((unsigned) (nh-nl+1)*sizeof(int));
   if (!v) nrerror("allocation failure in ivector()");
   return v-nl;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double *dvector(int nl, int nh)
{
   double *v;

   v=(double *)malloc((unsigned) (nh-nl+1)*sizeof(double));
   if (!v) nrerror("allocation failure in dvector()");
   return v-nl;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
char *cvector(int nl, int nh)
{
   char *v;

   v=(char *)malloc((unsigned) (nh-nl+1)*sizeof(char));
   if (!v) nrerror("allocation failure in cvector()");
   return v-nl;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double **dmatrix(int nrl, int nrh, int ncl, int nch)
{
   int i;
   double **m;

   m=(double **) malloc((unsigned) (nrh-nrl+1)*sizeof(double*));
   if (!m) nrerror("allocation failure 1 in dmatrix()");
   m -= nrl;

   for(i=nrl;i<=nrh;i++) {
      m[i]=(double *) malloc((unsigned) (nch-ncl+1)*sizeof(double));
      if (!m[i]) nrerror("allocation failure 2 in dmatrix()");
      m[i] -= ncl;
   }
   return m;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
char **cmatrix(int nrl, int nrh, int ncl, int nch)
{
   int i;
   char **m;

   m=(char **) malloc((unsigned) (nrh-nrl+1)*sizeof(char*));
   if (!m) nrerror("allocation failure 1 in cmatrix()");
   m -= nrl;

   for(i=nrl;i<=nrh;i++) {
      m[i]=(char *) malloc((unsigned) (nch-ncl+1)*sizeof(char));
      if (!m[i]) nrerror("allocation failure 2 in cmatrix()");
      m[i] -= ncl;
   }
   return m;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
float *vector(int nl, int nh)
{
   float *v;

   v=(float *)malloc((unsigned) (nh-nl+1)*sizeof(float));
   if (!v) nrerror("allocation failure in vector()");
   return v-nl;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
float **matrix(int nrl, int nrh, int ncl, int nch)
{
   int i;
   float **m;

   m=(float **) malloc((unsigned) (nrh-nrl+1)*sizeof(float*));
   if (!m) nrerror("allocation failure 1 in matrix()");
   m -= nrl;

   for(i=nrl;i<=nrh;i++) {
      m[i]=(float *) malloc((unsigned) (nch-ncl+1)*sizeof(float));
      if (!m[i]) nrerror("allocation failure 2 in matrix()");
      m[i] -= ncl;
   }
   return m;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void free_matrix(float **m, int nrl, int nrh, int ncl, int nch)
{
   int i;

   for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
   free((char*) (m+nrl));
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void free_cmatrix(char **m, int nrl, int nrh, int ncl, int nch)
{
   int i;

   for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
   free((char*) (m+nrl));
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void free_vector(float *v, int nl, int nh)
{
   free((char*) (v+nl));
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void free_dvector(double *v, int nl, int nh)
{
   free((char*) (v+nl));
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void free_cvector(char *v, int nl, int nh)
{
   free((char*) (v+nl));
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void free_dmatrix(double **m, int nrl, int nrh, int ncl, int nch)
{
   int i;

   for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
   free((char*) (m+nrl));
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void free_ivector(int *v, int nl, int nh)
{
   free((char*) (v+nl));
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
FILE   *openf(char fname[],char mode[])
{
   FILE   *f;

   if( (f = fopen(fname,mode)) == NULL) {
      printf("Cannot open file %s for %c%s%c.\n",fname,'"',mode,'"');
      printf("...exiting...\n");
      nrerror("Check your code");
      return 0;
   }
   else return(f);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
int RRemove(fname)
char *fname;
{
  int f;

  if( (f = remove(fname)) == -1  ) {
    printf("Unable to remove file %s\n",fname);
    printf("...exiting...\n");
    exit(1);
  }
  else return f;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
int Rename(oldfname,newfname)
char *oldfname;
char *newfname;
{
  int f;

  if( (f = rename(oldfname,newfname)) != 0 ) {
    printf("Unable to rename file %s to %s.\n",oldfname,newfname);
    printf("...exiting...\n");
    exit(1);
  }
  else return f;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double   atanh(x)
double   x;
{
   if( x <= -1.0 || x >= 1.0 ) nrerror("Error in function atanh() -1<x<1");
   return -0.5*log( (1-x)/(1+x) );
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double   Iden(x)
double   x;
{
   return x;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double   Inv(x)
double   x;
{
   if(x == 0.0) {
       nrerror("arg = 0.0 in Inv.");
       return 1.0;
   }
   else return 1.0/x;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double   logT1(x)
double   x;
{
   if(x == 0.0) {
       nrerror("arg = 0.0 in Inv.");
       return 1.0;
   }
   else return log10(1.0/x);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void dpolint(double xa[],double ya[],int n, double x, double *y, double *dy)
/*
Given arrays xa[1..n] and ya[1..n], and given a value x, this routine returns
an error estimate dy. If P(x) is the polinomial of degree n-1 such that P(xa)=
ya, i=1,...,n, then the returned value y=P(x).
*/
{
   int   i,m,ns=1;
   double   den,dif,dift,ho,hp,w;
   double   *c,*d,*dvector();
   void   nrerror(),free_dvector();

   dif=fabs(x-xa[1]);
   c=dvector(1,n);
   d=dvector(1,n);
   for (i=1;i<=n;i++) {
      if ( (dift=fabs(x-xa[i])) < dif) {
         ns=i;
         dif=dift;
      }
      c[i]=ya[i];
      d[i]=ya[i];
   }

   *y=ya[ns--];
   for (m=1;m<n;m++) {
      for (i=1;i<=n-m;i++) {
         ho=xa[i]-x;
         hp=xa[i+m]-x;
         w=c[i+1]-d[i];
         if ( (den=ho-hp) == 0.0) nrerror("Error in POLINT");
         den=w/den;
         d[i]=hp*den;
         c[i]=ho*den;
      }

      *y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));

   }
   free_dvector(d,1,n);
   free_dvector(c,1,n);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void dsplint(double xa[], double ya[], double y2a[], int n, double x, double *y)
{
   int klo,khi,k;
   double h,b,a;

   klo=1;
   khi=n;
   while (khi-klo > 1) {
      k=(khi+klo) >> 1;
      if (xa[k] > x) khi=k;
      else klo=k;
   }
   h=xa[khi]-xa[klo];
   if (h == 0.0) nrerror("Bad XA input to routine SPLINT");
   a=(xa[khi]-x)/h;
   b=(x-xa[klo])/h;
   *y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void dspline( double x[], double y[], int n, double yp1, double ypn, double y2[])
// double x[],y[],yp1,ypn,y2[];
// int n;
{
   int i,k;
   double p,qn,sig,un,*u,*dvector();
   void free_dvector();

   u=dvector(1,n-1);
   if (yp1 > 0.99e30)
      y2[1]=u[1]=0.0;
   else {
      y2[1] = -0.5;
      u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
   }
   for (i=2;i<=n-1;i++) {
      sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
      p=sig*y2[i-1]+2.0;
      y2[i]=(sig-1.0)/p;
      u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
      u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
   }
   if (ypn > 0.99e30)
      qn=un=0.0;
   else {
      qn=0.5;
      un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
   }
   y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
   for (k=n-1;k>=1;k--)
      y2[k]=y2[k]*y2[k+1]+u[k];
   free_dvector(u,1,n-1);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void new_line(f,n)
FILE *f;
int n;
{
  int i;
  char c;

  for(i=1;i<=n;i++) while( (c=fgetc(f)) != '\n');
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
#define IN  1
#define OUT 0

int swc(s)
char *s;
{
  int nw,i,state;
  char c;

  nw = i = 0;
  state = OUT;

  if(s == NULL) return 0;

  while((c=s[i++]) != 0){
    if(c == ' ' || c == '\n' || c == '\t' || c == EOF) state = OUT;
    else if(state == OUT){
      state = IN;
      ++nw;
    }
  }
  return nw;
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
int flc(file)
FILE *file;
{
  char str[1024]="";
  int lc=0;
  char *cerr=str;
  
  while(1){
    cerr = fgets(str,1024,file);
    if(swc(str)) lc++;
    if(str[0] == '#') lc--;
    if(!feof(file)) break;
  }
  rewind(file);
  if (cerr==NULL) printf("fgets read error in fitutil.c, flc().\n");
  return lc;
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void *Malloc(size)
unsigned int size;
{
  void *ptr;

  if((ptr = malloc(size)) != NULL) return ptr;
  else {
    puts("malloc alocation failure...exiting");
    exit(1);
  }
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void *Realloc(ptr,size)
void *ptr;
unsigned int size;
{
  if((ptr = realloc(ptr,size)) != NULL) return ptr;
  else {
    puts("realloc alocation failure...exiting");
    exit(1);
  }
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
int *Flc(file,nlines)
FILE *file;
int *nlines;
{
  char c,*str;
  int lc=0,*char_count,nchar=0;

  char_count = (int *) Malloc((unsigned) sizeof(int));
  while(1){
    str = (char *) Malloc((unsigned) sizeof(char));
    nchar=0;
    while(1){
      c = fgetc(file);
      str[nchar++]=c;
      str = (char *) Realloc(str,(unsigned) (nchar+1)*sizeof(char));
      str[nchar]=0;
      if(c=='\n' || feof(file)){
	break;
      }
    }
    if(swc(str) && str[0] != '#') {
      lc++;
      char_count = (int *) Realloc(char_count,(unsigned) lc*sizeof(int));
      char_count[lc-1]=nchar;
    }
    free((char *) str);
    if(feof(file)) break;
  }
  rewind(file);

  *nlines = lc;
  return char_count;
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
char **nlnc(nlinhas,nchar_linha)
int nlinhas,*nchar_linha;
{
  int i;
  char **opt;
  
  opt =(char **) Malloc((unsigned) (nlinhas+1)*sizeof(char*));
  for(i=0;i<nlinhas;i++) {
    opt[i]=(char *) Malloc((unsigned) (nchar_linha[i]+1)*sizeof(char));
  }
  opt[nlinhas]=NULL;
  return opt;
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void free_nlnc(m,nlinhas)
char **m;
int nlinhas;
{
   int i;

   for(i=0;i<=nlinhas+1;i++) free((char*) m[i]);
   free((char*) m);
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
char **lines(file,nlines,nchar_line)
FILE *file;
int nlines,*nchar_line;
{
  char c,*str;
  int lc=0,nchar=0;
  char **opt;

  
  opt = nlnc(nlines,nchar_line);
  
  while(1){
    str = (char *) Malloc((unsigned) sizeof(char));
    nchar=0;
    while(1){
      c = fgetc(file);
      if(c == '\n' || feof(file)) {
	str[nchar]=0;
	break;
      }
      else {
	str[nchar++]=c;
	str = (char *) Realloc(str,(unsigned) (nchar+1)*sizeof(char));
	str[nchar]=0;
      }
    }
    if(swc(str) && str[0] != '#') {
      lc++;
      strcpy(opt[lc-1],str);
    }
    free((char *) str);
    if(feof(file)) break;
  }
  rewind(file);

  return opt;
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
char **getsstring(str,n)
char *str;
int *n;
{
  int i,state,nw,nc;
  char c,**ptr;

  if((*n=swc(str))==0 || str == NULL) {
    return NULL;
  }

  ptr = (char **) Malloc((unsigned) (*n+1)*sizeof(char *));

  for(i=0;i<*n;i++) ptr[i] = (char *) Malloc((unsigned) sizeof(char));
  ptr[*n]=NULL;

  nw = nc = i = 0;
  state = OUT;

  while((c=str[i++]) != 0){
    if(c == ' ' || c == '\n' || c == '\t' || c == EOF) {
      state = OUT;
      nc = 0;
    }
    else if(state == OUT){
      state = IN;
      ++nw;
    }
    if(state == IN){
      ptr[nw-1][nc++] = c;
      ptr[nw-1] = (char *) Realloc(ptr[nw-1], (unsigned) (nc+1)*sizeof(char));
      ptr[nw-1][nc]=0;
    }
  }

  *n = nw;
  return ptr;

}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
char *purges(char *str)
{
  int n,m,state=OUT;

  n = m = strlen(str);

  while(--n >= 0) {
    if(str[n] == ' ' || str[n] == '\n' || str[n]=='\t') {
      str[n]=0;
      state = OUT;
    }
    else if(state == OUT) break;
  }
  for(n=0;n<m;n++) {
    if(str[n]!=' ' && str[n]!='\t') break;
  }
  return str+n;
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void puti(int i)
{
  printf("%d\n",i);
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void putd(double x)
{
  printf("%g\n",x);
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
double   ler_num(f)
FILE   *f;
{
   double   x;
   char   c;

   while( (c=fgetc(f)) != '=');
   if (!fscanf(f,"%lf",&x)) printf("fscanf() call error in fitutil.c, lernum()\n");
   return x;
}
/****************************************************************************/
/*                                                                           */
/*****************************************************************************/
int strplen(char **str)
{
  int i=0;
  
  while(str[0]!=NULL) i++;
  return i;
}

/****************************************************************************/
/*                                                                           */
/*****************************************************************************/
int *buffer_Flc(file,nlines)
FILE *file;
int *nlines;
{
  char c,*str;
  int lc=0,*char_count,nchar=0;

  char_count = (int *) Malloc((unsigned) sizeof(int));
  while(1){
    str = (char *) Malloc((unsigned) sizeof(char));
    nchar=0;
    while(1){
      c = fgetc(file);
      str[nchar++]=c;
      str = (char *) Realloc(str,(unsigned) (nchar+1)*sizeof(char));
      str[nchar]=0;
      if(c=='\n' || feof(file)){
	break;
      }
    }
    lc++;
    char_count = (int *) Realloc(char_count,(unsigned) lc*sizeof(int));
    char_count[lc-1]=nchar;
    free((char *) str);
    if(feof(file)) break;
  }
  rewind(file);

  *nlines = lc-1;
  return char_count;
}
/****************************************************************************/
/*                                                                           */
/*****************************************************************************/
char **buffer_lines(file,nlines,nchar_line)
FILE *file;
int nlines,*nchar_line;
{
  char c,*str;
  int lc=0,nchar=0;
  char **opt;

  
  opt = nlnc(nlines,nchar_line);
  
  while(1){
    str = (char *) Malloc((unsigned) sizeof(char));
    nchar=0;
    while(1){
      c = fgetc(file);
      if(c == '\n' || feof(file)) {
	str[nchar]=0;
	break;
      }
      else {
	str[nchar++]=c;
	str = (char *) Realloc(str,(unsigned) (nchar+1)*sizeof(char));
	str[nchar]=0;
      }
    }

    lc++;
    strcpy(opt[lc-1],str);

    free((char *) str);
    if(feof(file)) break;
  }
  rewind(file);

  return opt;
}
/****************************************************************************/
/*                                                                           */
/*****************************************************************************/





















