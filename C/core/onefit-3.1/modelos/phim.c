/******************************************************************************/
/*                              FIT_OUTPG.C                                   */
/******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "struct.h"
#include "plot.h"
#include "phim.h"

#define PIO2 1.57079632679490
/******************************************************************************/
/*				PHIM.C					      */
/******************************************************************************/
int main( int argc, char *argv÷[])
// int argc;
// char *argv[];
{
	double par[4];

    if(argc!=4) nrerror("param de entrada H0,K3/K1,niu");
	par[1] = atof(argv[1]);
	par[2] = atof(argv[2]);
	par[3] = atof(argv[3]);

	phiM_graf(par);
}
/******************************************************************************/
/*									      */
/******************************************************************************/
void    phiM_graf(double par)
// double  par[];
{

	Function X;
	double	K,phiM,niu,H0,f;

        int     k,nxy;

	char	typex[] = {"lin"};
	char	typey[] = {"lin"};

        float   tsizxs,tsizxb,tsizys,tsizyb;
        float   xm,xM,ym,yM,*x,*yt,*y;

	FILE	*file;

	file = openf("phim.gph","w");

	H0   = par[1];
	K  = par[2]-1.0;
	niu  = par[3];

	clear_struct(&X,5);
	X.par[0].low_v  = 0.0;
	X.par[0].high_v = 89.0*PIO2/90.0; 
	X.par[1].val    = H0;
	X.par[2].val    = K;
	X.par[3].val    = niu;
	w_f_ptr(&X,calc_phiM);
	
	nxy  = 100;

	xm   = 0.0;
	xM   = 5.0;
	ym   = 0.0;
	yM   = 2.0;

	x  = vector(0,nxy-1); 
	y  = vector(0,nxy-1); 
	yt = vector(0,nxy-1);

	if(!strcmp(typex,"log")) {
		fx = log10;
		tsizxs = -1.0;
		tsizxb = 1.0;
	}
	else if(!strcmp(typex,"logT1")) {
		fx = logT1;
		tsizxs = -1.0;
		tsizxb = 1.0;
	}
	else if(!strcmp(typex,"lin")) fx = Iden;
	else if(!strcmp(typex,"inv")) fx = Inv;
	else fx = sqrt;

	if(!strcmp(typey,"log")) {
		fy = log10;
		tsizys = -1.0;
		tsizyb = 1.0;
	}
	else if(!strcmp(typey,"logT1")) {
		fy = logT1;
		tsizys = -1.0;
		tsizyb = 1.0;
	}
	else if(!strcmp(typey,"T1")) fy = Inv;
	else if(!strcmp(typey,"inv")) fy = Inv;
	else fy = Iden;

	for(k=0;k<nxy;k++) {
		f = (xM-xm)*(double)k/(nxy-1.0)+xm;
		if(!strcmp(typex,"log")) f = pow(10.0,f);
		else if(!strcmp(typex,"sqrt")) f = pow(f,2.0);
		else f=f;

		x[k] = f;
		y[k] = PIO2;
		X.par[4].val = x[k]*H0;

		if(X.par[4].val <= H0) yt[k] = 0.0;
		else yt[k]= (float) szero(&X,0,1e-6);

		fprintf(file,"%e %e\n",x[k],yt[k]); 
/*		printf("f=%lg phiM=%lg\n",x[k],yt[k]); */
	}

	system("cls");
	cdevice(2);
	ctsetup();
	cerase();
	csetlim(xm,ym,xM,yM);
/*	cticksize(tsizxs,tsizxb,tsizys,tsizyb);*/
	cticksize(0.5,1.0,0.0,0.5);
	cbox(1,2); 
	cplot(nxy,fx,x,fy,yt,"l00");  
	cplot(nxy,fx,x,fy,y,"l10");  
	ctidle(); 

	free_vector(yt,0,nxy-1);
	free_vector(x,0,nxy-1);  
	fclose(file);
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
#undef PIO2
