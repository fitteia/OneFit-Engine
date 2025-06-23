/** ECD_poli.h **/


#define		FILE_J1TAB	"/home/lfx/lib/j1tabodf.dat"
#define		FILE_D2J1TAB	"/home/lfx/lib/d2J1tab.dat"
#define		TABSIZE_R	90
#define		TABSIZE_fr	72

void read_Jtab(char *tabfile, double *tabfr, double *tabR, double j1tab[TABSIZE_R][TABSIZE_fr]);
double J1odf_low(double A, double R, double wr);
double J1odf_low2(double A, double R, double wr,double v0);
double J1odf_lowR(double A, double R, double wr);
double J1odf_high(double A, double R, double wr);
double	J1odfcol(double wr, double R);
double	J1odfcolpoli(double wr, double R, double v0);
double ECD_poli(double f, double Aecd, double fcm, double fcM, double R, double v0);
double CED_poli(double f, double Aecd, double fcM, double R, double v0);

