#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "afactors-xyz.h"

#define NMAX    100

int  main(int argc, char **argv)
{
    FILE    *fout,*fin;
    void     Afactors(),r6medio();
    int      e1,e2;

    fin  = openf(argv[1],"r");
    fout = openf(argv[2],"w");

    if(argc<4) {
      r6medio(fin,fout);
      exit(1);
    }
    else {
      e1 = atoi(argv[3]);
      e2 = atoi(argv[4]);
    }
    Afactors(fin,fout,e1,e2);
    fclose(fin);
    fclose(fout);
    exit(1);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void Afactors(FILE *fin, FILE *fout, int e1, int e2)
// FILE    *fin,*fout;
// int e1,e2;
{
    int     c,i,j,natomos,nprotoes;
    double  b0,b1,b2;
    double  angulo(),distancia(),angij,rij,re1e2,A00,A0,A1,A2,tmp,tmp1=0;
    Atomo   A[500];

    fscanf(fin,"%d",&natomos);
    while( (c=fgetc(fin)) != '\n');
    for(i=0;i<natomos;i++){
      fscanf(fin,"%s%lf%lf%lf",A[i].nome,&A[i].x,&A[i].y,
                                   &A[i].z);
	A[i].numero=i+1;
/*      printf("%3d %10s %10.4lf %10.4lf %10.4lf\n",A[i].numero,A[i].nome,
               A[i].x,A[i].y,A[i].z);                                        */
    }

    re1e2 = distancia(A[e1-1],A[e2-1]);
    A0 = A1 = A2 = A00 = 0.0;
    nprotoes = 0;
    for(i=0;i<natomos;i++) {
        if(!strcmp(A[i].nome,"H")) ++nprotoes;
        for(j=i+1;j<natomos;j++){
            if(!strcmp(A[i].nome,"H") && !strcmp(A[j].nome,"H")){
	      tmp1++;
                   angij = angulo(A[i],A[j],A[e1-1],A[e2-1]);
                   rij   = distancia(A[i],A[j]);
                   tmp = 0.5*fabs((1-3*cos(angij)*cos(angij)))/pow(rij,3.0);
		   A00+=tmp;
                   b0 = 0.250*pow(1-3*cos(angij)*cos(angij),2.0)/pow(rij,6.0);
                   b1 = 0.250*3*sin(2*angij)*sin(2*angij)/pow(rij,6.0);
                   b2 = 0.250*3*pow(sin(angij),4.0)/pow(rij,6.0);
                   A0 += b0;
                   A1 += b1;
                   A2 += b2;

                   angij =angij*45.0/atan(1.0);
		   /*                   printf("proton number %4d\n",i+1); */
                   printf("%3d %3d %10.4lf %3d %3d %10.4lf %9.4lf %g %g %g %g\n",i+1,j+1,rij,e1,e2,re1e2,angij,tmp,b0,b1,b2);
/*
                   printf("%3d %3d %10.4lf %3d %3d %10.4lf %9.4lf",
                           i+1,j+1,rij,e1,e2,re1e2,angij);
			   printf(" %10.4lf %10.4lf %10.4lf",A0,A1,A2);*/
                   fprintf(fout,"%3d %3d %10.4lf %3d %3d %10.4lf %9.4lf %g %g %g %g\n",i+1,j+1,rij,e1,e2,re1e2,angij,tmp,b0,b1,b2);
		   /*                   fprintf(fout," %10.4lf %10.4lf %10.4lf\n",A0,A1,A2);*/

            }
        }
    }
    nprotoes--;
    printf("              Eixo  (%d,%d)\n",e1,e2);
    /*    printf(" <(1-3cos^2(a))/2r^3> = %le\n",A00/tmp1);*/
    /*    printf(" <(1-3cos^2(a))/2r^3> = %le\n",A00/nprotoes);*/
    printf(" <(1-3cos^2(a))^2/4r^6> = %le\n",A0/nprotoes);
    /*    printf(" <(1-3cos^2(a))^2/4r^6> = %le\n",A0/tmp1);*/
    printf("     <3sin^2(2a)/4r^6> = %le\n",A1/nprotoes);
    printf("     <3sin^4(a)/4r^6> = %le\n",A2/nprotoes);
    printf("%4d protoes\n",nprotoes+1);
    fprintf(fout,"\n\n                 Axis  (%d,%d)",e1,e2);
    /*    fprintf(fout,"\n\n <(1-3cos^2(a))/2r^3> = %le",A00);*/
    fprintf(fout,"\n <(1-3cos^2(a))^2/4r^6> = %le",A0/nprotoes);
    fprintf(fout,"\n      <3sin^2(2a)/4r^6> = %le",A1/nprotoes);
    fprintf(fout,"\n     <3sin^4(a)/4r^6> = %le",A2/nprotoes);
    fprintf(fout,"\n         nprotons = %4d",nprotoes+1);

/*    fprintf(fout,"\n A0 = %le",A0/nprotoes);
    fprintf(fout,"\n A1 = %le",A1/nprotoes);
    fprintf(fout,"\n A2 = %le",A2/nprotoes);
    fprintf(fout,"\n nprotoes = %d",nprotoes);
    fprintf(fout,"\n A00 = %le",A00*A00/nprotoes); */
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void r6medio( FILE *fin, FILE *fout)
// FILE    *fin,*fout;
{
    int     c,i,j,natomos,nprotoes;
    double  angulo(),distancia(),rij,r6;
    Atomo   A[500];

    fscanf(fin,"%d",&natomos);
    while( (c=fgetc(fin)) != '\n');
    for(i=0;i<natomos;i++){
      fscanf(fin,"%s%lf%lf%lf",A[i].nome,&A[i].x,&A[i].y,
                                   &A[i].z);
	A[i].numero=i+1;

	/*        fscanf(fin,"%d%s%lf%lf%lf%lf",&A[i].numero,A[i].nome,&A[i].x,&A[i].y,
		  &A[i].z,&lixo);*/
/*      printf("%3d %10s %10.4lf %10.4lf %10.4lf\n",A[i].numero,A[i].nome,
               A[i].x,A[i].y,A[i].z);                                        */
    }

    r6 = 0.0;
    nprotoes = 0;
    for(i=0;i<natomos;i++) {
        if(!strcmp(A[i].nome,"H")) ++nprotoes;
        for(j=i+1;j<natomos;j++){
            if(!strcmp(A[i].nome,"H") && !strcmp(A[j].nome,"H")){
                   rij = distancia(A[i],A[j]);
                   r6  += 1.0/pow(rij,6.);;
		   //                   printf("proton number %3d\n",i+1);
               
                   printf("%3d %3d %10.4lf %10.4lf\n", i+1,j+1,rij,r6);
		   fprintf(fout,"%3d %3d %10.4lf %10.4lf\n", i+1,j+1,rij,r6);
               
            }
        }
    }
    nprotoes--;
    r6 /= nprotoes;
    printf("             %d protons\n",nprotoes+1);
    printf("<1/r^6>   = %le\n",r6);
    fprintf(fout,"\nnprotons = %d",nprotoes+1);
    fprintf(fout,"\n<1/r^6>   = %le\n",r6);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double  distancia(Atomo A1, Atomo A2)
// Atomo   A1,A2;
{
    double d;

    d = sqrt( pow(A1.x-A2.x,2.0) + pow(A1.y-A2.y,2.0) + pow(A1.z-A2.z,2.0) );
    return d;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
double  angulo(Atomo A1, Atomo A2, Atomo A3, Atomo A4)
// Atomo   A1,A2,A3,A4;
{
    double  distancia(),ang,d12,d34,v;

    d12 = distancia(A1,A2);
    d34 = distancia(A3,A4);

    v = (A2.x-A1.x)*(A4.x-A3.x)+(A2.y-A1.y)*(A4.y-A3.y)+(A2.z-A1.z)*(A4.z-A3.z);
    ang = acos(v/(d12*d34));

    return ang;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
FILE *openf(char fname[], char modei[])
// char    fname[];
// char    mode[];
{
        FILE    *fopen();
        FILE    *f;

        if( (f = fopen(fname,mode)) == NULL) {
                printf("Cannot open file %s for %c%s%c.\n",fname,'"',mode,'"');
                printf("...exiting...\n");
                exit(1);
        }
        else return(f);
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
