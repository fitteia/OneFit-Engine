#include <math.h>
#include <stdio.h>
#include "globals.h"
#include <string.h>
#include "funcoes.h"

double _x0(double x,double *par,double t,int mode)
// double x,*par,t;
// int mode;
{
  return 1.0;
}

double _x1(double x,double *par,double t,int mode)
// double x,*par,t;
// int mode;
{
  return x;
}

double _x2(double x,double *par,double t,int mode)
// double x,*par,t;
// int mode;
{
  return x*x;
}

double _x3(double x,double *par,double t,int mode)
// double x,*par,t;
// int mode;
{
  return x*x*x;
}

double _x4(double x,double *par,double t,int mode)
// double x,*par,t;
// int mode;
{
  return pow(x,4.0);
}

double _x5(double x,double *par,double t,int mode)
// double x,*par,t;
// int mode;
{
  return pow(x,5.0);
}

double _x6(double x,double *par,double t,int mode)
// double x,*par,t;
// int mode;
{
  return pow(x,6.0);
}

static int _xn_first_time=YES;
static int _xn_arg_a=-1;

double _xn(double x,double *par,double t,int mode,char **par_names,int nnames)
// double x,*par,t;
// int mode,nnames;
// char **par_names;
{
  if(par_names != NULL){
    if(nnames<1) gfitn_error("exp: ","needs 1 fitting parameter as argument\n");
    if(_xn_first_time){
	GETPAR(_xn_arg_a,par_names[0]);
	_xn_first_time=NO;
    }
  }
  else {
    _xn_arg_a=1;
  }
  return pow(x,par[_xn_arg_a]);
}

static int _exp_first_time=YES;
static int _exp_arg_a=-1;

double _exp(double x,double *par,double t,int mode,char **par_names,int nnames)
// double x,*par,t;
// int mode,nnames;
// char **par_names;
{
  if(par_names != NULL){
    if(nnames<1) gfitn_error("exp: ","needs 1 fitting parameter as argument\n");
    if(_exp_first_time){
	GETPAR(_exp_arg_a,par_names[0]);
	_exp_first_time=NO;
    }
  }
  else {
    _exp_arg_a=1;
  }


  return exp(par[_exp_arg_a]*x);
}

double PerlNLF(double x,double *par,double t,int mode,char *name)
// double x,*par,t;
// int mode;
// char name[];
{
  char **parametros;
  double result;
  int i,j,npar;

  npar=par[0];

  parametros = cmatrix(0,2*npar+3,0,20);

  sprintf(parametros[0],"%3.20f",x);
  sprintf(parametros[1],"%3.20f",t);
  sprintf(parametros[2],"%3.20d",mode);

  for(i=3,j=0;i<2*npar+3;i += 2,j++){
      sprintf(parametros[i],"%s",ParNames[j]);
      sprintf(parametros[i+1],"%3.20f",par[j]);
  }
  parametros[2*npar+3]=NULL;
  result = perl_func_call(name, parametros);

  free_cmatrix(parametros,0,2*npar+3,0,20);

  return result;
}


double PerlLF(double x,double *par,double t,int mode,char *name)
// double x,*par,t;
// int mode;
// char name[];
{
  char **parametros;
  double result;

  parametros = cmatrix(0,3,0,20);

  sprintf(parametros[0],"%3.20f",x);
  sprintf(parametros[1],"%3.20f",t);
  sprintf(parametros[2],"%3.20d",mode);
  parametros[3]=NULL;
  result = perl_func_call(name, parametros);
  free_cmatrix(parametros,0,3,0,20);

  return result;
}
