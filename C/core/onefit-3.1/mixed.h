/**********************
#include <stdio.h>
#include <math.h>
#include <stdlib.h>				
#include <string.h>				
#include "globals.h"				
#include "struct.h"				
#include "userlib.h"				
***********************/
#include <unistd.h>						
#ifdef __APPLE__				
#include <sys/types.h>			
#include <sys/sysctl.h>			
#endif
#include <pthread.h>
#include <spawn.h>
#include <sys/wait.h>
#include <fcntl.h>

static int get_max_workers_(void)									
{																	
    int workers = 1;												
    																
#ifdef __APPLE__													
    int mib[2] = {CTL_HW, HW_NCPU};									
    size_t len = sizeof(workers);									
    if (sysctl(mib, 2, &workers, &len, NULL, 0) != 0 || workers < 1)	
        workers = 1;													
#else																	
    long n = sysconf(_SC_NPROCESSORS_ONLN);								
    if (n > 0) workers = (int)n;										
#endif 																	
        																
    char *env = getenv("ONEFITE_WORKERS");								
    if (env && *env) {													
        int x = atoi(env);												
        if (x > 0) workers = x;											
    }																	

    return workers;														
}

extern char **environ;

static int run_fit_spawn(int k)
{
    pid_t pid;
    int status;

    char datafile[64], parfile[64], outfile[64], logfile[64];

    sprintf(datafile, "data%d.dat", k);
    sprintf(parfile,  "fit%d.par",  k);
    sprintf(outfile,  "fit%d.out",  k);
    sprintf(logfile,  "fit%d.log",  k);

    /* open files */
    int fd_in  = open(parfile, O_RDONLY);
    int fd_out = open(logfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd_in < 0 || fd_out < 0) {
        perror("open");
        return -1;
    }

    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);

    /* stdin  <- fit.par */
    posix_spawn_file_actions_adddup2(&actions, fd_in, STDIN_FILENO);

    /* stdout -> fit.log */
    posix_spawn_file_actions_adddup2(&actions, fd_out, STDOUT_FILENO);

    /* stderr -> same log */
    posix_spawn_file_actions_adddup2(&actions, fd_out, STDERR_FILENO);

    /* arguments (NO shell!) */
    char *argv[] = {
        PROGNAME,
        "-@fitenv.stp",
        datafile,
        "-f",
        "-np",
        "-ng",
        "-o",
        outfile,
        NULL
    };

    int ret = posix_spawn(&pid, PROGNAME, &actions, NULL, argv, environ);

    posix_spawn_file_actions_destroy(&actions);

    close(fd_in);
    close(fd_out);

    if (ret != 0) {
        perror("posix_spawn");
        return -1;
    }

    waitpid(pid, &status, 0);

    return status;
}

#define pi 3.1415926						
#define PI 3.1415926						
#define pI 3.1415926						
#define Pi 3.1415926						

#define SET_FIT_METHODS(...) const char *fit_methods[] = (const char*[]){__VA_ARGS__};

#define PAR(no,n) ( (MIXED) ? pars_[(int) no][(int) n+1] : par[(int) n] )	

int write_parfile(double *a);
int update_pars_(double *p);
int print_pars_(double *p);
int First_Make_Individual_Fits(double N, double *par, double no);

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
    FILE *fin, *fout;					\
    char s[256], val[11];					\
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
		strcpy(val,"");						\
	    sscanf(&s[40],"%s",val);				\
	    fprintf(fout,"%-10s",val);				\
		strcpy(val,"");							\
	    sscanf(&s[50],"%s",val);				\
	    fprintf(fout,"%-10s",val);				\
		strcpy(val,"");							\
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
      fprintf(fout,"%-10s",val);      				\
      fprintf(fout,"%-10.3e\n",0.0);     /**** set MIXED = 0.0 *****/	\
      fgets(s, sizeof(s),fin);						\
      fprintf(fout,"%s",s);						\
 									\
      int numMethods = sizeof(fit_methods) / sizeof(fit_methods[0]);	\
									\
      while( !feof(fin) ) {						\
	char fix[11];							\
	double no;							\
	fgets(s, sizeof(s),fin);					\
	sscanf(s,"%s",fix);						\
	if(!strcmp(fix,"fix")) {					\
	  sscanf(&s[10],"%lf",&no);					\
	  /* printf("%lg\n",no); */					\
	  if ( !mixed[(int) no-1] ) fprintf(fout,"%s",s);		\
	}								\
	else if (!strcmp(fix,"set") ) fprintf(fout,"%s",s);		\
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
									\
/* ---- parallel helpers ---- */	\
								\
static int next_k_ = 1;				\
static pthread_mutex_t next_k_mutex_ = PTHREAD_MUTEX_INITIALIZER;	\
																	\
																	\
static int use_parallel_update_pars_(void)							\
{																	\
    char *env = getenv("ONEFITE_NO_PARALLEL");						\
																	\
    if (env && *env && atoi(env) != 0) return 0;					\
																	\
    if (NT <= 2) return 0;											\
																	\
    return 1;														\
}																	\
																	\
static void *update_pars_worker_(void *arg)							\
{																	\
    (void)arg;														\
    \
    while (1) {														\
        int k;														\
																	\
        pthread_mutex_lock(&next_k_mutex_);							\
        k = next_k_++;												\
        pthread_mutex_unlock(&next_k_mutex_);						\
																	\
        if (k > NT) break;											\
	\
        FILE *fin;													\
        char s[256];												\
																	\
        /*sprintf(s,"%s -@fitenv.stp data%d.dat -f -np -ng -ofit%d.out <fit%d.par >fit%d.log 2>&1", PROGNAME,k,k,k,k); */ \
							\
        /*system(s);*/					\
	run_fit_spawn(k);				\
        sprintf(s,"fit%d.out",k);	\
        fin = openf(s,"r");			\
									\
        if( fgets(s,sizeof(s),fin) == NULL) nrerror("fgets failure");	\
        if( fgets(s,sizeof(s),fin) == NULL) nrerror("fgets failure");	\
																		\
        for(int i=1;i<=NP;i++)											\
            pars_[k][i]=ler_num(fin);									\
																		\
        fclose(fin);													\
    }																	\
																		\
    return NULL;														\
}																		\
																		\
int update_pars_(double *p) \
{							\
    FILE *fin;				\
    char s[256];			\
							\
    if(MIXED) {				\
							\
        if (!use_parallel_update_pars_()) {	\
											\
            /* ---- ORIGINAL SERIAL VERSION ---- */	\
            for( int k=1;k<=NT;k++) {				\
	      /*sprintf(s,"%s -@fitenv.stp data%d.dat -f -np -ng -ofit%d.out <fit%d.par >fit%d.log 2>&1", PROGNAME,k,k,k,k);*/ \
					\
	      /*system(s);*/				\
	      run_fit_spawn(k);				\
                sprintf(s,"fit%d.out",k);	\
                fin = openf(s,"r");			\
											\
                if( fgets(s,sizeof(s),fin) == NULL) nrerror("fgets failure in AuxCode");	\
                if( fgets(s,sizeof(s),fin) == NULL) nrerror("fgets failure in AuxCode");	\
																							\
                for(int i=1;i<=NP;i++)			\
                    pars_[k][i]=ler_num(fin);	\
												\
                fclose(fin);					\
            }									\
												\
        } else {								\
												\
            /* ---- PARALLEL VERSION ---- */	\
            int max_workers = get_max_workers_();	\
            if (max_workers > NT) max_workers = NT;	\
            if (max_workers < 1) max_workers = 1;	\
													\
            pthread_t *threads = malloc(max_workers * sizeof(*threads));	\
            if (!threads) nrerror("malloc failed");			\
															\
            next_k_ = 1;									\
															\
            for (int i = 0; i < max_workers; i++) pthread_create(&threads[i], NULL, update_pars_worker_, NULL);	\
																				\
            for (int i = 0; i < max_workers; i++) pthread_join(threads[i], NULL); 	\
									\
            free(threads);			\
        }							\
    }								\
    else {							\
        for( int i=0; i<=NT; i++) {		\
            for( int j=0; j<=NP; j++) {	\
                pars_[i][j]= (i == 0) ? j : (j == 0) ? i : p[j-1];	\
            }	\
        }	\
    }	\
		\
    return 1;	\
}	\
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
