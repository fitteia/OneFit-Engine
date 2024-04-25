#ifdef WITH_PERL 
#include <EXTERN.h>
#include <perl.h>
#endif

#define YES 1
#define NO 0

extern char *version_number;

typedef struct {
  const char *name;
  int has_arg;
  int *flag;
  int val;
} option;

typedef struct foption {
  char *name;
  double (*ptr)();
  int flag;
  char *comment;
} Foption;

extern option long_options[];
extern char short_options[];

extern char **myoptions;
extern Foption *foptions;
extern int first_time;
extern char *funcao;

typedef struct foption *Foption_ptr;
typedef struct foption **Foption_vector_ptr;

extern int xy_flag;
extern int xyz_flag;
extern int xyt_flag;
extern int xscale_flag;
extern int yscale_flag;
extern int point_type_flag;
extern int gph_flag;
extern int num_gph_points_flag;
extern int function_flag;
extern int autox_flag;
extern int autoy_flag;
extern int fit_flag;
extern int plot_flag;
extern int graphic_flag;
extern int parameter_flag;
extern int outdev_flag;
extern int label_flag;
extern int nolinpar_flag;
extern int errors_flag;
extern int list_flag;
extern int xmgr_flag;
extern int xmgrace_flag;
extern int grbatch_flag;
extern int display_flag;

extern char *GRAPH_TYPE;
extern char **XMGR_PAR_FILE;
extern char **xscale;
extern char **yscale;
extern char **point_type;
extern char **gph_name;
extern char **xlabel;
extern char **ylabel;
extern char *device;
extern char *outdev;
extern char *res_output;
extern char *parfile;
extern char ***AA_names;
extern char **dfiles;
extern char *FUNCAO;
extern double *xm;
extern double *xM;
extern double *ym;
extern double *yM;
extern double _X;
extern double _Y;
extern double _E;
extern int *num_gph_points;
extern double (**function)();
extern char *Gnuplot;
extern int Nfuncoes;
extern int Ngnus;

extern int Ma,Mfit,*Ndata,*Lista,NT;
extern double ***VData,*_T,**AA,**Covar;
extern double **Data;

extern int FPar;
extern int *ParFreeVector;
extern char **ParNames;
extern char **Func_names;
extern char ***Func_args;
extern int *Func_nargs;
extern FILE *pointers_file;

extern void nrerror();
extern void gfitn_error();
extern int *ivector();
extern double *dvector();
extern double **dmatrix();
extern char **cmatrix();
extern char *cvector();
extern float *vector();
extern float **matrix();
extern void free_matrix();
extern void free_cmatrix();
extern void free_vector();
extern void free_dvector();
extern void free_cvector();
extern void free_dmatrix();
extern void free_ivector();
extern FILE   *openf();
extern int RRemove();
extern int Rename();
extern double   atanh();
extern double   Iden();
extern double   Inv();
extern double   logT1();
extern void dpolint();
extern void dsplint();
extern void dspline();
extern void new_line();
extern int swc();
extern int flc();
extern int *Flc();
extern char **nlnc();
extern void free_nlnc();
extern char **lines();
extern void *Malloc();
extern void *Realloc();
extern char **getsstring();
extern void initialize();
extern void user_initialize();
extern char *purges();
extern void puti();
extern void putd();
extern double ler_num();
extern int strplen();
extern int *buffer_Flc();
extern char **buffer_lines();
extern void mygnus();
extern void xmgr();
extern void grbatch();
extern double perl_func_call();
extern double PerlLF();
extern double PerlNLF();
extern double Ftotal();

#define GETPAR(a,b)\
({							\
      int j;						\
      for(j=0;j<par[0];j++){				\
	  if(!strcmp(ParNames[j],(b))){                 \
	      (a)=j;                                    \
	      break;                                    \
	  }                                             \
      }                                                 \
      if((a)<0) gfitn_error("GETPAR: ","one of your arguments is not matched by any parameter in the Parameter file");\
})


#define LINPAR 0
#define NOLINPAR 1

#define NEW_ENTRY(a,b,c,d) \
({ \
    double (b)();\
    foptions[Nfuncoes].name = (a); \
    foptions[Nfuncoes].ptr  = (b); \
    foptions[Nfuncoes].flag = (c); \
    foptions[Nfuncoes].comment = (d); \
    Nfuncoes++; \
    foptions = (Foption *) Realloc(foptions,(unsigned) (Nfuncoes+1)*sizeof(Foption)); \
    fprintf(pointers_file,"%s %p %d # %s\n",(a), (b), (c), (d)); \
    if(list_flag) printf("%s %p %d # %s\n",(a), (b), (c), (d)); \
})   

#define START_INITIALIZE \
({\
    pointers_file = openf("gfitn.ptr","a");\
})

#define CLOSE_INITIALIZE \
({\
    printf("%d functions in memory during the gfitn run.\n",Nfuncoes);\
    fclose(pointers_file);\
})

#define CALLOFF(function,x,y,par_names,number_parameters)( (function)((x),par,(y),0,(par_names),(number_parameters)) )

#define FTOTAL(x,y)( Ftotal((x),(y),par) )
