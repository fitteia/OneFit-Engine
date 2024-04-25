#define YES 1
#define NO 0
#define myMAX(a,b) ((a) > (b) ? (a) : (b))
#define myMIN(a,b) ((a) < (b) ? (a) : (b))

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

typedef struct foption *Foption_ptr;
typedef struct foption **Foption_vector_ptr;

char version_number[]="3.1";

int xy_flag=1;
int xyz_flag=0;
int xyt_flag=0;
int xscale_flag=0;
int yscale_flag=0;
int point_type_flag=0;
int gph_flag=0;
int num_gph_points_flag=1;
int function_flag=0;
int autox_flag=1;
int autoy_flag=1;
int fit_flag=1;
int display_flag=1;
int plot_flag=0;
int graphic_flag=1;
int parameter_flag=0;
int outdev_flag=0;
int label_flag=0;
int nolinpar_flag=0;
int errors_flag=0;
int list_flag=0;
int xmgr_flag=0;
int xmgrace_flag=0;
int grbatch_flag=0;

int FPar;
int *ParFreeVector;
char **ParNames;

int Nfuncoes=0;
int Ngnus=0;
char *GRAPH_TYPE=0;
char **XMGR_PAR_FILE=0;
char **xscale=0;
char **yscale=0;
char **point_type=0;
char **gph_name=0;
char **xlabel=0;
char **ylabel=0;
char *device=0;
char *outdev=0;
char *res_output=0;
char *parfile=0;
char *Gnuplot=0;
char ***AA_names=0;
char **dfiles=0;
char *FUNCAO=0;
double *xm=0;
double *xM=0;
double *ym=0;
double *yM=0;
double _X=0;
double _Y=0;
double _E=0;
int *num_gph_points=0;
FILE *pointers_file=0;

static option long_options[]={
  {"autox_scale",0,&autox_flag,1},
  {"autox",0,&autox_flag,1},
  {"autoy_scale",0,&autoy_flag,1},
  {"autoy",0,&autoy_flag,1},
  {"fit",0,&fit_flag,1},
  {"nofit",0,&fit_flag,0},
  {"plot",0,&plot_flag,1},
  {"noplot",0,&plot_flag,0},
  {"label",0,&label_flag,1},
  {"nolabel",0,&label_flag,0},
  {"errors",0,&errors_flag,1},
  {"device",1,0,1},
  {"Outdev",1,0,2},
  {"xlabel",1,0,3},
  {"ylabel",1,0,4},
  {"xscale",1,0,5},
  {"yscale",1,0,6},
  {"point_type",1,0,7},
  {"ptype",1,0,7},
  {"gph",1,0,8},
  {"num_gph_points",1,0,9},
  {"num",1,0,9},
  {"Function",1,0,10},
  {"Parameter",1,0,11},
  {"Par",1,0,11},
  {"xm",1,0,12},
  {"xM",1,0,13},
  {"ym",1,0,14},
  {"yM",1,0,15},
  {"Gnuplot",1,0,16},
  {"Gnu",1,0,16},
  {"xy",0,0,17},
  {"xyz",0,0,18},
  {"xyt",0,0,19},
  {"xmgr",0,0,20},
  {"xmgr_par_file",1,0,21},
  {"PERL_file",1,0,22},
  {"grbatch",1,0,23},
  {"xmgrace",0,0,24},
  {"display",0,0,25},
  {0,0,0,0}
};

const char short_options[]="F:G:O:P:d:o:@:n:ftpzleghD";

char **myoptions;
Foption *foptions;
int first_time=YES;
char *funcao;
char **Func_names;
char ***Func_args;
int *Func_nargs;

double (**function)()=0;

int Ma=0,Mfit=0,*Ndata,*Lista,NT=0;
double ***VData=0,*_T=0,**AA=0,**Covar=0;
double **Data;

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
extern void ParFreeF();
extern char **ReadParNames();
extern void minuit_();
extern void minuit_MSDOS();
extern void fcn();
extern double gchi2_vn();
extern void usage();
extern void gfit_outgraf();
extern void inigfit();
extern void plotfitn();
extern int *buffer_Flc();
extern char **buffer_lines();
extern void mygnus();
extern void xmgr();
extern void grbatch();
extern double PerlLF();
extern double PerlNLF();



#define START_INITIALIZE \
({\
    pointers_file = openf("gfitn.ptr","w");\
    foptions = (Foption *) Malloc((unsigned) sizeof(Foption));\
    Nfuncoes = 0;\
})

#define CLOSE_INITIALIZE \
({\
    printf("%d functions in memory during the gfitn run.\n",Nfuncoes);\
    fclose(pointers_file);\
})

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

#define LINPAR 0
#define NOLINPAR 1







