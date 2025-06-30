/** rot.h  **/

double	rot(double t, double w, double delta);
double	Pij(int i, int j, double S,double P4);
double  JR(int k, double w, double S, double P4, double ts, double tl, double A0, double A1, double A2);
double	rot_reor(double ts, double tl, double w, double delta, double S, double P4, double A0, double A1, double A2);
double	rot_reor_poli(double ts, double tl, double w, double S, double P4, double A0, double A1, double A2);
double	rot_reor_ro(double ts, double tl, double w1, double w, double delta, double S, double P4, double A0, double A1, double A2);
double  rot_reor_cad_poli(double w, double tauS, double tauL, double S, double P4, double tauSc, double tauLc, double Sc, double P4c, double A0, double A1, double A2);
double	Jcad(int k, double w, double tauS, double tauL, double S, double P4, double tauSc, double tauLc, double Sc, double P4c, double A0, double A1, double A2);
double	d2ij2(int i, int j, double S, double P4);
