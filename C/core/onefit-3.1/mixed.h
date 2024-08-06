/**********************
#include <stdio.h>
#include <math.h>
#include <stdlib.h>				
#include <string.h>				
#include "globals.h"				
#include "struct.h"				
#include "userlib.h"				
***********************/
#define pi 3.1415926						
#define PI 3.1415926						
#define pI 3.1415926						
#define Pi 3.1415926						

#define FIT_METHODS(...) const char *fit_methods[] = (const char*[]){__VA_ARGS__};

#define PAR(no,n) ( (MIXED) ? pars_[(int) no][(int) n+1] : par[(int) n] )	

#define SET_INDIVIDUAL_FITS_ENGINE(N,par)			\
								\
  static int FIRST_TIME=1;			                \
  static int INIT=1;						\
  static int No=0;						\
  static int NP;						\
  static int MIXED=0;						\
								\
  static double **pars_;					\
								\
  int write_parfile(double *p)					\
  {								\
    FILE *fin, *fout, *openf();					\
    char s[256], val[11];					\
    int update_pars_(), print_pars_();				\
    int *mixed;							\
								\
    mixed = ivector(0,NP);					\
								\
    fin   = openf("fit.par","r");				\
								\
    for( int i=1; i<=NT; i++) {					\
      sprintf(s,"fit%d.par", i);				\
      fout = openf(s,"w");					\
								\
      fgets(s, sizeof(s),fin);					\
      fprintf(fout,"%s",s);					\
      fgets(s, sizeof(s),fin);					\
      fprintf(fout,"%s",s);					\
      for (int k=1; k < NP-1; k++) {				\
	fgets(s, sizeof(s),fin);				\
	if ( swc(s)-1 >= 3 ){					\
	  sscanf(s,"%s",val);					\
	  fprintf(fout,"%-10s",val);				\
	  sscanf(&s[10],"%s",val);				\
	  fprintf(fout,"%-10s",val);				\
	  fprintf(fout,"%-10.3e",p[k]);				\
	  mixed[k]=0;						\
	  if( val[strlen(val)-1] == '_' ) {			\
	    mixed[k]=1;						\
	    /* printf("%d\n",k);	*/			\
	    sscanf(&s[30],"%s",val);				\
	    fprintf(fout," %-9s",val);				\
	    sscanf(&s[40],"%s",val);				\
	    fprintf(fout,"%-10s",val);				\
	    sscanf(&s[50],"%s",val);				\
	    fprintf(fout,"%-10s",val);				\
	  }							\
	  fprintf(fout,"\n");					\
	} else {						\
	  fprintf(fout,"%s",s);					\
	}							\
      }								\
      fgets(s, sizeof(s),fin);					\
      sscanf(s,"%s",val);					\
      fprintf(fout,"%-10s",val);				\
      sscanf(&s[10],"%s",val);					\
      fprintf(fout,"%-10s",val);					\
      fprintf(fout,"%-10.3e\n\n",0.0);     /**** set MIXED = 0.0 *****/	\
									\
      int numMethods = sizeof(fit_methods) / sizeof(fit_methods[0]);	\
									\
      while( !feof(fin) ) {						\
	char fix[11];							\
	double no;							\
	fgets(s, sizeof(s),fin);					\
	sscanf(s,"%s",fix);						\
	printf("%s %d\n",fix,strlen(fix));				\
	if(!strcmp(fix,"fix")) {					\
	  sscanf(&s[10],"%lf",&no);					\
	  /* printf("%lg\n",no); */					\
	  if ( !mixed[(int) no-1] ) fprintf(fout,"%s",s);		\
	}								\
	else if (!strcmp(fix,"set") ) fprintf(fout,"%s",s);		\
	else if (strlen(fix) < 2 ) fprintf(fout,"%s",s);		\
	else {								\
	  if (numMethods<1) fprintf(fout,"%s",s);			\
	};								\
      }									\
									\
      if (numMethods > 0) {						\
	/* printf("sizeoffit_methods)=%lu sizeof(pointer)=%lu numStrings=%d\n",sizeof(fit_methods),sizeof(fit_methods[0]),numMethods); */ \
	for (int i = 0; i < numMethods; i++) {				\
	  fprintf(fout,"%s\n",fit_methods[i]);				\
	  /* printf("%s ",fit_methods[i]); */				\
	}								\
	fprintf(fout,"exit\n");						\
	/* printf("\n"); */						\
      }									\
      fflush(fout);							\
      fclose(fout);							\
      rewind(fin);							\
    }									\
    fclose(fin);							\
    free_ivector(mixed,0,NP);						\
									\
    return 1;								\
  }									\
									\
  int print_pars_(double *p)						\
  {									\
    for( int i=0; i<=NT; i++) {						\
      for( int j=0; j<=NP; j++) {					\
	printf("%lg ", pars_[i][j]);					\
      }									\
      printf("\n");							\
    }									\
    printf("\n");							\
    return 1;								\
  }									\
									\
  int update_pars_(double *p)						\
  {									\
    FILE *fin,*openf();							\
    char s[256];							\
    int print_pars_();							\
									\
    if(MIXED) {								\
      for( int k=1;k<=NT;k++) {						\
	sprintf(s,"%s -@fitenv.stp data%d.dat -f -np -ng -ofit%d.out <fit%d.par >fit%d.log 2>&1",PROGNAME,k,k,k,k); \
	/*      printf("%s\n",s);  */					\
	system(s);							\
	sprintf(s,"fit%d.out",k);					\
	fin = openf(s,"r");						\
	if( fgets(s,sizeof(s),fin) == NULL) nrerror("fgets failure in AuxCode"); \
	if( fgets(s,sizeof(s),fin) == NULL) nrerror("fgets failure in AuxCode"); \
	for(int i=1;i<=NP;i++) pars_[k][i]=ler_num(fin);		\
	fclose(fin);							\
	/*      print_pars_(p); */					\
      }									\
    }									\
    else {								\
      for( int i=0; i<=NT; i++) {					\
	for( int j=0; j<=NP; j++) {					\
	  pars_[i][j]= (i == 0) ? j : (j == 0) ? i : p[j-1];		\
	}								\
      }									\
    }									\
    return 1;								\
  }									\
									\
									\
  int First_Make_Individual_Fits(double N, double *par, double no)      \
  {									\
    int write_parfile();						\
    int print_pars_();							\
    int update_pars_();							\
									\
    NP       = (int) par[0];						\
    MIXED = (int) par[NP-1];						\
									\
    if(INIT) {								\
      pars_=dmatrix(0,NT,0, NP);					\
      for( int i=0; i<=NT; i++) {					\
	for( int j=0; j<=NP; j++) {					\
	  pars_[i][j]= (i == 0) ? j : (j == 0) ? i : par[j-1];		\
	}								\
      }									\
      /*    print_pars_(par); */					\
      INIT=0;								\
    }									\
									\
    if (MIXED) {							\
      if (FIRST_TIME) {							\
	/*      printf("first time %lg %d %d\n",N_0,No,NT);*/		\
	write_parfile(par);						\
	update_pars_(par);						\
	FIRST_TIME=0;							\
	No = (int) no;							\
	return 1;							\
      }									\
      else {								\
	if ( no < No ) {						\
	  FIRST_TIME=1;							\
	  return First_Make_Individual_Fits(N,par,no);			\
	}								\
	else No = (int) no;						\
	return 0;							\
      }									\
    }									\
    else {								\
      update_pars_(par);						\
      return 0;								\
    }									\
  }									
