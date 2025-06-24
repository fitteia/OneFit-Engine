/** fitutil.h **/

void nrerror(char error_text[]);
double *dvector(int nl, int nh);
void dpolint(double xa[],double ya[],int n, double x, double *y, double *dy);
void free_dvector(double *v, int nl, int nh);
void dsplint(double xa[], double ya[], double y2a[], int n, double x, double *y)
