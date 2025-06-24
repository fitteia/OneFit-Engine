/** fitutil.h **/

int RRemove(char *fname);
void *Malloc(unsigned int size);
void *Realloc(void *ptr,unsigned int size);
int Rename(char *oldfname, char *newfname);
int *Flc(FILE *file, int nlines);
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

