/** fitutil.h **/

FILE *openf(char fname[],char mode[]);
char **buffer_lines(FILE *file, int nlines, int *nchar_line);
char **getsstring(char *str, int *n);
char **lines(FILE *file, int nlines, int *nchar_line);
char **nlnc( int nlinhas, int *nchar_linha);
char *purges(char *str);
double *dvector(int nl, int nh);
double Iden( double x);
double Inv( double x);
double ler_num(FILE *f);
double logT1( double x);
int *Flc(FILE *file, int *nlines);
int *buffer_Flc(FILE *file, int *nlines);
int *ivector(int nl, int nh);
int RRemove(char *fname);
int Rename(char *oldfname, char *newfname);
int flc(FILE *file);
int strplen(char **str);
int swc(char *s);
void *Malloc(unsigned int size);
void *Realloc(void *ptr,unsigned int size);
void dpolint(double xa[],double ya[],int n, double x, double *y, double *dy);
void dspline( double x[], double y[], int n, double yp1, double ypn, double y2[]);
void dsplint(double xa[], double ya[], double y2a[], int n, double x, double *y);
void free_dvector(double *v, int nl, int nh);
void free_ivector(int *v, int nl, int nh)
void free_nlnc(char **m, int nlinhas);
void new_line(FILE *f, int n);
void nrerror(char error_text[]);
void putd(double x);
void puti(int i);
