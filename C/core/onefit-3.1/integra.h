/** integra..h **/

double	r_pval(Function	*x, int n);
double	r_plow(Function *x, int n);
int	r_n_par(Function *x);
void	wsval(Function *x);
void	w_f_ptr(Function *x, double (*f)(Function *x));
void	w_plow(Function	*x, int n, double lv);
double	r_phigh(Function *x, int n);
void	w_phigh(Function *x, int n, double hv);
double	r_pstep(Function *x, int n);
void	w_pstep(Function *x, int n, double sv);
void	w_pstep(Function *x, int n, double sv);
void	clear_struct(Function *f_struct, int n_par);
double smidpnt(Function *X, int p, int n);
double	sqromo(Function *X,double (*choose)(Function *a, int b, int c), int p);
double sqgaus(Function *X, int p);
void gauleg(double x1, double x2, double x[], double w[], int n);
double sqgausn(Function *X, int p, int n);
double	szero(Function *X, int n, double eps);

