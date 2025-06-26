// fitk_util.h

double	r_phigh(Function *x, int n);
double	r_plow(Function *x, int n);
double	r_pstep(Function *x, int n);
double	r_pval(Function *x, int n);
double	sqromo(Function *X, double (*choose)(Function *x), int p);
double smidpnt(Function *X, int p, int n);
int	r_n_par(Function *x);
void	clear_struct(Function *f_struct, int n_par);
void	w_f_ptr(Function *x,double		(*f)(Function *a));
void	w_phigh(Function *x, int n, double hv);
void	w_plow(Function *x, int n, double lv);
void	w_pstep(Function *x, int n, double sv);
void	w_pval(Function *x, int n, double v);
void	wsval(Function *X);
