// fft-utils.h

double pow_fft(double Re_data[], double Im_data[], int npoints, double step);
void area_fft(double Re_data[], double Im_data[], int npoints, double step, double *a_Re, double *a_Im);
void dfour1(double data[], int nn, int isign);
void drealfft(double data[], int n, int isign);
void dtwofft(double data1[], double data2[], double fft1[], double fft2[], int n);
void expri(double data[], double re[], double im[], int n);
void exri(double data[], double re[], double im[], int n);
