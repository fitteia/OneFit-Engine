/** odf.h **/

double	odfN(double wcm, double wcM, double w, double delta);
double	Jodf_k123(double K13, double K23, double eta12, double RM, double Rm, double wcM, double wcm, double w);
double	odfN_ro(double wcm, double wcM, double w, double w1, double delta);
double	odf_k123( double w, double delta, double K13, double K23, double eta12, double RM, double Rm, double wcM, double wcm);
double	odfSa(double wcm, double wcM, double w, double delta);
double	int_godfSN_v1(Function *X);
double	JodfN(double w, double aM, double am);
double	godfSN_v1( double alfa, double w, double Wc, double Ws, double Ka3, double K13, double eta1a, double R, double BD);
double	odfSa_ro(double wcm, double wcM, double w, double w1, double delta);
double	JodfSN_v1( double BD, double K13, double K23, double eta12, double RM, double Rm, double wcM, double wcm, double ws, double w);
double	JodfSa(double w, double aM, double am);
double	odfSN_v1( double w, double delta, double K13, double K23, double eta12, double RM, double Rm, double wcM, double wcm, double ws, double BD);
double	godfN(double a);
double	int_godfSN(Function *X);
double	godfSa(double a);
double	godfSN( double alfa, double w, double Wc, double Wb, double Ka1, double K31, double eta1a, double R, double Q);
double	godf_k123(double wc_w, double eta1a, double Ka3, double R);
double	JodfSN( double Q, double K31, double K21, double eta12, double RM, double Rm, double wcM, double wcm, double wb, double w);
double	godfa(double ZaB, double n, double m1, double m2);
double	odfSN( double w, double delta, double K31, double K21, double eta12, double RM, double Rm, double wcM, double wcm, double wb, double Q);
double	odfS_K1B( double w, double delta, double wcM, double wcm, double wb);
double	JodfS_K1B( double wcM, double wcm, double wb, double w);



