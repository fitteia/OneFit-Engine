/** RNordio.h **/

double RNordio( double f,  double S0, double A0, double A1, double A2,  double Dz,  double Dx);
double RNordio_ang( double f,  double S0, double A0, double A1, double A2,  double Dz,  double Dx, double delta);
double JRotNordio_(int mL, double w, double P2, double A0, double A1, double A2, double Dz, double Dx);
double JRotNordio_2(int mL, double w, double P2, double A0, double A1, double A2, double Dz, double Dx);
double itau_(int mL,int mM, double Dz, double Dx,double P2);
double beta_(int mL, int mM, double P2);
double c_(int mL, int mM, double P2);

