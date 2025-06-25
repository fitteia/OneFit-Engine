/******************************************************************************/
/*									      */
/******************************************************************************/
#define	FUNC(x)		(((x)->f_ptr)(x))
#define	Func(x)		((*func)(x))
#define	Funk(x)		((*func)(1.0/(x))/((x)*(x)))

/******************************************************************************/
/*									      */
/******************************************************************************/
typedef	struct {
	char	name[20];
	int	status;  /* 0 - fix        */
			 /* 1 - free       */
			 /* 2 - integrate  */
			 /* 3 - cycling    */			
			 /* 4 - serie      */
			 /* 5 - fft        */
	double	low_v;
	double	high_v;
	double	step_v;
	double	val;
	double	min_v;
} Parameter;

/******************************************************************************/
/*									      */
/******************************************************************************/
struct Function;

typedef	struct Function {
	char		name[20];
	int		status; /* 0 - not choosed */
				/* 1 - choosed     */
	int		n_par;
	double		(*f_ptr)(struct Function *x);                 /* function pointer */
	void            *P;
	Parameter	par[20];
} Function;

/******************************************************************************/
/*									      */
/******************************************************************************/
typedef struct {
	double	x;
	double	y;
} Ponto;
/******************************************************************************/
/*									      */
/******************************************************************************/
typedef	union {
	unsigned u;
	int	 i;
	long	 l;
	float	 f;
	double	 d;
	char	 c;
	void	*p;
} VAR;
/******************************************************************************/
/*									      */
/******************************************************************************/
typedef	struct {
	VAR	name;
	VAR	status;  /* 0 - fix        */
			 /* 1 - free       */
			 /* 2 - integrate  */
			 /* 3 - cycling    */			
			 /* 4 - serie      */
			 /* 5 - fft        */
	VAR	lv;
	VAR	hv;
	VAR	sv;
	VAR	v;
	VAR	mv;
} PARAMETER;

/******************************************************************************/
/*									      */
/******************************************************************************/
typedef	struct	  {
	VAR		name;
	VAR		status; /* 0 - not choosed */
				/* 1 - choosed     */
	VAR		np;
	VAR		f;                 /* function */
	PARAMETER	par[10];
} FUNCTION;

/******************************************************************************/
/*									      */
/******************************************************************************/
struct ODE2;

typedef struct ODE2 {
  double             x0;
  double             y0;
  double             z0;
  double             x;
  double             y;
  double             z;
  double             h;
  double             (*dydx)(struct ODE2 *a);                 /* function */
  double             (*d2ydx2)(struct ODE2 *b);                 /* function */
  Parameter          par[10];
} ODE2;
