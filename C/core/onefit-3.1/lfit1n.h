// lfit1n.h

void lfit1n( double x[], double y[], double sig[], int ndata, double a[], int ma, int lista[], 
			 int mfit, double *chisq,
			 void (*func_ptr)(double x[],double *afunc, int ma, double t, int mode), 
			 double par[], double t, int mode);

