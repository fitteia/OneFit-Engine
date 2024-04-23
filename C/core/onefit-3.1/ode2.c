#include <math.h>
#include <stdio.h>
#include "globals.h"
#include "struct.h"


static double k[]={0,0,0,0};
static double l[]={0,0,0,0};
static double x[]={0,0};
static double y[]={0,0};
static double z[]={0,0};

static int ruku4thode2_first_time=1;

double RuKu4thODE2(xxx,ode2)
double xxx;
ODE2 *ode2;
{
  double h,af;
  
  h=ode2->h;

  if(ruku4thode2_first_time==1){
    x[0]=x[1]=ode2->x0;
    y[0]=y[1]=ode2->y0;
    z[0]=z[1]=ode2->z0;
    ruku4thode2_first_time=0;
  }
  
  if(xxx<=x[0]){
    x[0]=x[1]=ode2->x0;
    y[0]=y[1]=ode2->y0;
    z[0]=z[1]=ode2->z0;
  }
  
  do {
    x[0]=x[1];
    y[0]=y[1];
    z[0]=z[1]; 
    
    ode2->x=x[0];      
    ode2->y=y[0];      
    ode2->z=z[0];      
    
    k[0]=h*ode2->dydx(ode2);
    l[0]=h*ode2->d2ydx2(ode2);
    
    ode2->x=x[0]+0.5*h;      
    ode2->y=y[0]+0.5*k[0];      
    ode2->z=z[0]+0.5*l[0];      

    k[1]=h*ode2->dydx(ode2);
    l[1]=h*ode2->d2ydx2(ode2);

    ode2->x=x[0]+0.5*h;      
    ode2->y=y[0]+0.5*k[1];      
    ode2->z=z[0]+0.5*l[1];      

    k[2]=h*ode2->dydx(ode2);
    l[2]=h*ode2->d2ydx2(ode2);

    ode2->x=x[0]+h;      
    ode2->y=y[0]+k[2];      
    ode2->z=z[0]+l[2];      

    k[3]=h*ode2->dydx(ode2);
    l[3]=h*ode2->d2ydx2(ode2);

    x[1]=x[0]+h;
    y[1]=y[0]+(k[0]+2*k[1]+2*k[2]+k[3])/6.0;
    z[1]=z[0]+(l[0]+2*l[1]+2*l[2]+l[3])/6.0;
    }
    while( xxx>=x[1] );
    af=(y[1]-y[0])/(x[1]-x[0])*(xxx-x[0])+y[0];

    return af;   
}

