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
char *PROGNAME=0;
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
extern double PerlLF();
extern double PerlNLF();
extern double Ftotal();
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


extern void initialize();
extern void user_initialize();
extern void ParFreeF();
extern double perl_func_call();
extern char **ReadParNames();
extern void minuit_();
extern void minuit_MSDOS();
extern void fcn();
extern double gchi2_vn();
extern void usage();
extern void gfit_outgraf();
extern void inigfit();
extern void plotfitn();
extern double PerlLF();
extern double PerlNLF();

void read_arguments(int argc, char *argv[]);
void decode_funcao(char *optarg);
double perl_func_call(char s[],char *spointer[]);

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







