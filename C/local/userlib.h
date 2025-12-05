/** Bcirc.h **/

double _Bcirc_z(Function *X);
double _Bcirc_x(Function *X);
double Bcirc(double x, double z, double miu, double N, double R, double I, double NP, double flag);
/** BPP.h **/


double BPP(double f,double a,double tauc);
/** CompElliptic1.h **/


double _elliptic1_(Function *X);
double CompElliptic1(double K, double NP);

/** CROSSRELAX.h **/

double CROSSRELAX(double f, double a, double tau, double f0);
double CROSSR1(double f, double a, double tau, double f0);
double R1CRsp1(double f, double Cnh, double tq, double fq1, double fq2, double Theta, double Phi);

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

/** ECDpoli.h **/


double ECDpoli(double f, double Aecd, double fcm, double fcM, double R, double n);
double J1ecdpoli( double w, double R, double wcm, double wcM, double n);
double jecd_int(Function *X);
double jecd_int1(Function *X);
double j1ecd(double R,double w,double wcm,double wcM, double n);
double jecd_int2(Function *X);
double j1ecd1(double w,double R,double wc,double n);

/** EllipticE.h **/


double _elliptice_(Function *X);
double EllipticE(double x, double K, double NP);

/** EllipticF.h  **/


double _ellipticf_(Function *X);
double EllipticF(double x, double K, double NP) ;

/** GAULOR.h **/

double GAULOR(double f, double a, double b, double f0, double t);

/** GAUSS.h **/



double GAUSS(double f, double a, double b, double f0, double Df);

/** GAUSS1.h **/


double GAUSS1(double f, double a, double b, double f0);

/** GaussLegendreInt.h **/


double GaussLegendreInt(Function *X,int p,int n);

/** HavNeg.h **/


double JHN(double w, double t, double epsilon, double delta);
double HavNeg(double f,double A,double tau, double epsilon, double delta);

// iT1inISpara.h

double iT1inISpara(double f, double T, double td, double ts, double r,double S);
// iT1ISpara.h

double iT1ISpara(double f, double T, double td, double ts,double M, double r,double S);
double J0ISpara_infts(double w, double td);
double J0ISpara(double w, double td, double ts);

// iT1isSmallS.h

double __Jin(double w, double td, double tdg, double S);
double iT1isSmallS(double f, double T, double tmg, double tmH, double tv, double ZFS, double r,double Sp,double S);

// iT1osSamllS.h

double iT1osSmallS(double f, double T, double D, double tv,double ZFS, double r,double Sp);

/** JacobiCN.h **/


double JacobiCN(double u, double k, double ni);

/** JacobiDN.h **/

double JacobiDN(double u, double k, double ni);



/** JacobiSN.h **/

double JacobiSN(double u, double k, double ni);


/** LORTZN.h **/

double LORTZN(double f, double a, double b, double f0);


/** MNPDanuta.h **/

double _Bs(double wS, double S, double T);
double _j_int1MP(Function *X);
double _jMP(double a, double b, double NP);
double mnpR1HSc(double f, double T, double gH, double gS, double C, double S, double d, double D, double NP);
double mnpR1Hszn(double f, double T, double gH, double gS, double C, double S, double d, double D, double T1e, double NP);
double mnpR1Hsze(double f, double T, double gH, double gS, double C, double S, double d, double D, double T2e, double NP);


/** ODFN.h **/

double ODFN(double f, double Aodf, double fcm, double fcM, double delta);

/** ODFSmA.h **/

double ODFSmA(double f, double Alu, double fcm, double fcM, double delta);


/** OPF.h **/

double jopf_int(Function *X);
double jopf(double w,double tau0,double wcm,double wcM,double n);
double OPF(double f, double Aopf, double f0,double fcm, double fcM,double n);


/** R1ISsbm.h **/

double R1ISsbm(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh);

/** R1LipSzab.h **/

double R1LipSzab(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh, double SLS, double tl);


/** R1OSabhf.h **/

double R1OSabhf(double f, double C, double S, double a, double D, double tv, double Delta2);
double jhf(double re, double im);

/** R1Torrey.h **/

double R1Torrey(double f, double n, double d, double r, double tau);
double JTorrey(double f, double n, double d, double r, double tau);
double jTorrey(double alpha, double wt);

/** RMTDnum.h **/


double RMTDnum(double f, double p, double Armtd, double fcm,double fcM);
double jrmtd_int1(Function *X);
double jrmtd(double f,double fcM,double fcm,double p);
double jrmtd_int(Function *X);

/** RNordio.h **/

double RNordio( double f,  double S0, double A0, double A1, double A2,  double Dz,  double Dx);
double RNordio_ang( double f,  double S0, double A0, double A1, double A2,  double Dz,  double Dx, double delta);
double JRotNordio_(int mL, double w, double P2, double A0, double A1, double A2, double Dz, double Dx);
double JRotNordio_2(int mL, double w, double P2, double A0, double A1, double A2, double Dz, double Dx);
double itau_(int mL,int mM, double Dz, double Dx,double P2);
double beta_(int mL, int mM, double P2);
double c_(int mL, int mM, double P2);

/** RotVold.h **/

double RotVold(double f,double delta, double Szz, double tauS,  double tauL, double k1, double p, double A0, double A1, double A2);
/** RotWoessner.h **/


double RotWoessner( double f,  double S, double A0, double A1, double A2, double  Dx,  double Dz, double delta);
double JRotWoessner_(int mL, double w, double P2, double A0, double A1, double A2, double Dx, double Dz);
/** ROUSE.h **/

double ROUSE(double f,double a,double tauc);


/** ROUSE123.h **/

double ROUSE123(double f, double a, double f0, double f1, double p0, double p1, double p2);


/** ROUSE1234.h **/

double ROUSE1234(double f, double a1, double f0,double f1, double f2, double p1, double p2, double p3, double p4);
/** SDFreed.h **/

double SDFreed(double f, double tau, double n, double d, double NP);
double SDFreedHF(double f1, double f2, double tau, double n, double d, double NP);
double __JSDFreed_(double f, double tau, double NP);
double __QSDFreed_(double wt, double NP) ;
double __integral0_(Function *X);
/** SDN11_2_DSllDp2_poli.h **/

double SDN11_2_DllDp2_poli(double f, double d, double n, double tauD);
/** SDN11_2_DllDp2.h **/

double SDN11_2_DllDp2(double f, double d, double n, double tauD, double delta);

// SDvilfSmB.h

double _iT1wSmB(int k, double t, double w);
double SDvilfSmB(double f, double d, double n, double t);
/** Torrey.h **/

double Torrey(double f,double d,double n,  double tauD);
/** Torrey1.h **/

double Torrey1(double f,double d,double r, double n,  double tauD);
// Umath.h

double factorial(double x);

/** florence_c.h **/

double Florence( double FREQ, double SI, double GAMMAI, double SPIN, double IREL, double DELTA2, double TAURM, double TAUVM, double TAUMM, double DPARAM, double EPARAM, double S4M, double GXM, double GYM, double GZM, double AXM, double AYM, double AZM, double DM, double DDM, double CONCM, double ACQ, double AMOLFRAM, double RKM, double ACONTM, double THETAM, double PHIM);

/** florence_f.h **/

void florence_(double PINP[27], double *FREQ, double R1[10]);

