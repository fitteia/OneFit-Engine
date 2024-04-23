#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define	pi	3.141592654

/****************************   T1RODELTA.C      **************************/


int main()
{
	int	np;
	double	f0,f1,td,d,n,A,fcm,fcM,RA,RB;
	char	file[20];
	FILE	*f,*openf();
	void	graf();
	
	printf(" input file: ");
	scanf("%s",file);

	f = openf(file,"r");

	fscanf(f,"%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%d",
		&f0,&f1,&td,&d,&n,&A,&fcm,&fcM,&RA,&RB,&np);
/*
	printf("f0=%lg\nf1=%lg\ntd=%lg\nd=%lg\nn=%g\nA=%lg\nfcm=%lg\nfcM=%lg\n",
		f0,f1,td,d,n,A,fcm,fcM);
	printf("RA=%lg\nRB=%lg\nnxy=%d\n",RA,RB,np);	
*/
	graf(f0,f1,td,d,n,A,fcm,fcM,RA,RB,np);	
	
	fclose(f);		
}

void	graf(f0,f1,td,d,n,A,fcm,fcM,RA,RB,nxy)
int	nxy;
double	f0,f1,td,d,n,A,fcm,fcM,RA,RB;
{
	int	k;
	double	w0,w1,wcm,wcM,delta;
        float   *vector(),*x,*y1,*y2,*y3,*yt;
        double  Iden(),t1ro7(),odfN_ro();

        void    free_vector();
        void    cdevice(),ctsetup(),cerase(),csetlim(),cbox(),cconnect();
        void    csetangle(),cpoints(),csetexpand(),crelocate(),ctidle();
        void    setltype(),cticksize();
        void    cplot();

	w0 =2*pi*f0;
	w1 =2*pi*f1;
	wcm=2*pi*fcm;
	wcM=2*pi*fcM;

	system("cls");
	cdevice(2);
	ctsetup();

	x   = vector(0,nxy-1); 
	y1  = vector(0,nxy-1);
	y2  = vector(0,nxy-1);
	y3  = vector(0,nxy-1);
	yt  = vector(0,nxy-1);

	cerase();
	csetlim(0.0,0.0,90.0,10.0);
	cticksize(5.0,10.0,1.0,2.0);
	cbox(1,2); 
	for(k=0; k<nxy; k++) {
		delta= 0.5*pi*k/(nxy-1);
		x[k] = 180/pi*delta;
		y1[k]= t1ro7(0.5,d,n,td,w1,w0,delta);
		y2[k]= A*odfN_ro(wcm,wcM,w0,w1,delta);
		y3[k]= RA+RB-0.5*RB*pow(sin(delta),2.0);
		yt[k]= y1[k]+y2[k]+y3[k];		
	}
	cplot(nxy,Iden,x,Iden,y1,"l10");  
	cplot(nxy,Iden,x,Iden,y2,"l20");  
	cplot(nxy,Iden,x,Iden,y3,"l30");  
	cplot(nxy,Iden,x,Iden,yt,"l00");  
	ctidle(); 

	free_vector(x,0,nxy-1);
	free_vector(y1,0,nxy-1);
	free_vector(y2,0,nxy-1);
	free_vector(y3,0,nxy-1);
	free_vector(yt,0,nxy-1);
	
}

