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
extern char *PROGNAME;
extern double *xm;
extern double *xM;
extern double *ym;
extern double *yM;
extern double _X;
extern double _Y;
extern double _E;
extern int *num_gph_points;
extern double (**function)(double x, double par[], double t, int mode, char **args, int  nargs);
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

extern void nrerror(char a[]);
extern void gfitn_error(char error_text[],char option_msg[]);
int *ivector(int nl, int nh);
extern double *dvector(int nl, int nh);
extern double **dmatrix(int nrl, int nrh, int ncl, int nch);
extern char **cmatrix(int nrl, int nrh, int ncl, int nch);
extern char *cvector(int nl, int nh);
extern float *vector(int nl, int nh);
extern float **matrix(int nrl, int nrh, int ncl, int nch);
extern void free_matrix(float **m, int nrl, int nrh, int ncl, int nch);
extern void free_cmatrix(char **m, int nrl, int nrh, int ncl, int nch);
extern void free_vector(float *v, int nl, int nh);
extern void free_cvector(char *v, int nl, int nh);
extern void free_dmatrix(double **m, int nrl, int nrh, int ncl, int nch);
void free_ivector(int *v, int nl, int nh);
double   atanh(double a);
extern void dpolint(double xa[], double ya[], int n, double x, double *y, double *dy);
extern void dsplint(double *xa, double *ya, double *y2a, int n, double x, double *y);
extern void initialize();
extern void user_initialize();
extern void mygnus();
void xmgr(char prog[], char format[]);
extern void grbatch();
double perl_func_call(char s[],char *spointer[])
double PerlLF(double x,double *par,double t,int mode,char *name)
double PerlNLF(double x,double *par,double t,int mode,char *name)
double   Ftotal(double x, double t, double *p);
int RRemove(char *fname);
void *Malloc(unsigned int size);
void *Realloc(void *ptr,unsigned int size);
int Rename(char *oldfname, char *newfname);
int *Flc(FILE *file, int *nlines);
char **nlnc( int nlinhas, int *nchar_linha);
FILE *openf(char fname[],char mode[]);
void free_nlnc(char **m, int nlinhas);
double Iden( double x);
char **lines(FILE *file, int nlines, int *nchar_line);
char **getsstring(char *str, int *n);
void nrerror(char error_text[]);
char *purges(char *str);
double Inv( double x);
void puti(int i);
void putd(double x);
double *dvector(int nl, int nh);
double ler_num(FILE *f);
double logT1( double x);
int strplen(char **str);
int *buffer_Flc(FILE *file, int *nlines);
void dpolint(double xa[],double ya[],int n, double x, double *y, double *dy);
char **buffer_lines(FILE *file, int nlines, int *nchar_line);
void dspline( double x[], double y[], int n, double yp1, double ypn, double y2[]);
void free_dvector(double *v, int nl, int nh);
void new_line(FILE *f, int n);
void dsplint(double xa[], double ya[], double y2a[], int n, double x, double *y);
int swc(char *s);
int flc(FILE *file);

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
