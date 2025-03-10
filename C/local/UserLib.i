%module UserLib

%inline %{
  extern double BPP(double f,double a,double tauc);
  extern double ODFN(double f, double Aodf, double fcm, double fcM, double delta);
  extern double OPF(double f, double Aopf, double f0,double fcm, double fcM,double n);
  extern double ODFSmA(double f, double Alu, double fcm, double fcM, double delta);
  extern double SDN11_2_DllDp2(double f, double d, double n, double tauD, double delta);
  extern double SDN11_2_DllDp2_poli(double f, double d, double n, double tauD);
  extern double SDvilfSmB(double f, double d, double n, double t);
  extern double R1Torrey(double f,double n,double d,  double r, double tauD); 
  extern double Torrey(double f,double d,double n,  double tauD);
  extern double Torrey1(double f,double d,double r, double n,  double tauD);
  extern double GAUSS(double f, double a, double b, double f0, double Df);
  extern double GAUSS1(double f, double a, double b, double f0);
  extern double GAULOR(double f, double a, double b, double f0, double t);
  extern double GLmix(double f, double a, double b, double f0, double t);
  extern double LORTZN(double f, double a, double b, double f0);
  extern double ROUSE(double f,double a,double tauc);
  extern double ROUSE123(double f, double a1, double f0,double f1, double p1, double p2, double p3);
  extern double ECD_poli(double f, double Aecd, double fcm, double fcM, double R, double v0);
  extern double CED_poli(double f, double Aecd, double fcM, double R, double v0);
  extern double RMTDnum(double f, double p, double Armtd, double fcm,double fcM);
  extern double RotVold(double f,double delta, double Szz, double tauS,  double tauL, double k1, double p, double A0, double A1, double A2);
  extern double RNordio(double f, double S0, double A0, double A1, double A2, double Dz, double Dx);
  extern double RNordio_poli(double f, double S0, double A0, double A1, double A2, double Dz, double Dx);
  extern double RNordio_ang(double f, double S0, double A0, double A1, double A2, double Dz, double Dx, double delta);
  extern double RotWoessner(double f, double S0, double A0, double A1, double A2, double ts, double tl,double delta);
  extern double CROSSRELAX(double f, double a, double tau, double f0);
  extern double CROSSR1(double f, double a, double tau, double f0);
  extern double R1CRsp1(double f, double Cnh, double tq, double fq1, double fq2, double Theta, double Phi);
  extern double iT1ISpara(double f, double T, double td, double ts,double M, double r,double S);
  extern double iT1inISpara(double f, double T, double td, double ts, double r,double S);
  extern double iT1isSmallS(double f, double T, double tmg, double tmH, double tv, double ZFS, double r,double Sp,double S);
  extern double iT1osSmallS(double f, double T, double D, double tv,double ZFS, double r,double Sp);
  extern double mnpR1HSc(double f, double T, double gH, double gS, double C, double S, double d, double D, double NP);
  extern double mnpR1Hszn(double f, double T, double gH, double gS, double C, double S, double d, double D, double T1e, double NP);
  extern double mnpR1Hsze(double f, double T, double gH, double gS, double C, double S, double d, double D, double T2e, double NP);
  extern double R1ISsbm(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh);
  extern double jhf(double w, double re, double im);
  extern double R1OSabhf(double f, double C, double S, double a, double D, double tv, double Delta2);
  extern double CompElliptic1(double K,double NP);
  extern double EllipticF(double x, double k,double NP);
  extern double EllipticE(double x, double k,double NP);
  extern double JacobiSN(double u, double k, double ni);
  extern double JacobiCN(double u, double k, double ni);
  extern double JacobiDN(double u, double k, double ni);
  extern double HavNeg(double x, double A,double tau, double epsilon, double delta);
  extern double SDFreed(double x,double d,double n,  double tauD, double NP);
  extern double SDFreedHF(double x1 double x2,double d,double n,  double tauD, double NP);
  extern double Bcirc(double x, double z, double miu, double N, double R, double I, double NP, double axis);
  extern double Bcoil(double x, double z, double miu, double nz, double nr, double dz, double R, double I, double NP, double flag);
  extern double R1LipSzab(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh, double SLS, double tl);
%}

extern double BPP(double f,double a,double tauc);
extern double ODFN(double f, double Aodf, double fcm, double fcM, double delta);
extern double OPF(double f, double Aopf, double f0,double fcm, double fcM,double n);
extern double ODFSmA(double f, double Alu, double fcm, double fcM, double delta);
extern double SDN11_2_DllDp2(double f, double d, double n, double tauD, double delta);
extern double SDN11_2_DllDp2_poli(double f, double d, double n, double tauD);
extern double SDvilfSmB(double f, double d, double n, double t);
extern double R1Torrey(double f,double n,double d,  double r, double tauD); 
extern double Torrey(double f,double d,double n,  double tauD);
extern double Torrey1(double f,double d,double r, double n,  double tauD);
extern double GAUSS(double f, double a, double b, double f0, double Df);
extern double GAUSS1(double f, double a, double b, double f0);
extern double GAULOR(double f, double a, double b, double f0, double t);
extern double GLmix(double f, double a, double b, double f0, double t);
extern double LORTZN(double f, double a, double b, double f0);
extern double ROUSE(double f,double a,double tauc);
extern double ROUSE123(double f, double a1, double f0,double f1, double p1, double p2, double p3);
extern double ECD_poli(double f, double Aecd, double fcm, double fcM, double R, double v0);
extern double CED_poli(double f, double Aecd, double fcM, double R, double v0);
extern double RMTDnum(double f, double p, double Armtd, double fcm,double fcM);
extern double RotVold(double f,double delta, double Szz, double tauS,  double tauL, double k1, double p, double A0, double A1, double A2);
extern double RNordio(double f, double S0, double A0, double A1, double A2, double Dz, double Dx);
extern double RNordio_ang(double f, double S0, double A0, double A1, double A2, double Dz, double Dx, double delta);
extern double RNordio_poli(double f, double S0, double A0, double A1, double A2, double Dz, double Dx);
extern double RotWoessner(double f, double S0, double A0, double A1, double A2, double ts, double tl, double delta);
extern double CROSSRELAX(double f, double a, double tau, double f0);
extern double CROSSR1(double f, double a, double tau, double f0);
extern double R1CRsp1(double f, double Cnh, double tq, double fq1, double fq2, double Theta, double Phi);
extern double iT1ISpara(double f, double T, double td, double ts,double M, double r,double S);
extern double iT1inISpara(double f, double T, double td, double ts, double r,double S);
extern double iT1isSmallS(double f, double T, double tmg, double tmH, double tv, double ZFS, double r,double Sp,double S);
extern double iT1osSmallS(double f, double T, double D, double tv,double ZFS, double r,double Sp);
extern double mnpR1HSc(double f, double T, double gH, double gS, double C, double S, double d, double D, double NP);
extern double mnpR1Hszn(double f, double T, double gH, double gS, double C, double S, double d, double D, double T1e, double NP);
extern double mnpR1Hsze(double f, double T, double gH, double gS, double C, double S, double d, double D, double T2e, double NP);
extern double R1ISsbm(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh);
extern double jhf(double w, double re, double im);
extern double R1OSabhf(double f, double C, double S, double a, double D, double tv, double Delta2);
extern double CompElliptic1(double K,double NP);
extern double EllipticF(double x, double k, double NP);
extern double EllipticE(double x, double k, double NP);
extern double JacobiSN(double u, double k, double ni);
extern double JacobiCN(double u, double k, double ni);
extern double JacobiDN(double u, double k, double ni);
extern double HavNeg(double x, double A,double tau, double epsilon, double delta);
extern double SDFreed(double x,double d,double n,  double tauD, double NP);
extern double SDFreedHF(double x1 double x2,double d,double n,  double tauD, double NP);
extern double Bcirc(double x, double z, double miu, double N, double R, double I, double NP, double axis);
extern double Bcoil(double x, double z, double miu, double nz, double nr, double dz, double R, double I, double NP, double flag);
extern double R1LipSzab(double f, double q, double C, double ms, double rho, double S, double tm, double tR, double tv, double r, double Delta2, double Aoh, double SLS, double tl);
