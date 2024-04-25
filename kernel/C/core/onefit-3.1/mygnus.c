#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

#define GNUPLOT_3_5_beta_328 1

int v_flag;

char set[]="set";
char term[]="terminal";
char forma[]="format";
char size[]="size";
char logs[]="logscale";
char clip[]="clip";
char one[]="one";
char pausa[]="pause";
char plt[]="plot";
char us[]="using";
char wl[]="with lines";

void mygnus()
{
  FILE *f,*openf();
  char ins[2048],lixo[100],Files_gph[100];
  char lixo1[100],aux[100],dlixo[100],dlixo1[100],gph[100],gnu[100];
  int i,j,k,nchar;
  char *typex,*typey;
  char pt[10];
  void new_line();
  int print_data();

  if(Gnuplot == NULL) f = openf("gfitn.gnu","w");
  else f = openf(Gnuplot,"w");

  if(device != NULL){
    if(v_flag==1) printf("%s %s %s\n",set,term,device);
    fprintf(f,"%s %s %s\n",set,term,device);
  }
  if(outdev != NULL) {
    if(v_flag==1) printf("%s output\"%s\"\n",set,outdev);
    fprintf(f,"%s output \"%s\"\n",set,outdev);
  }
  if(v_flag==1) printf("#%s %s xy \"%%3.0e\"\n",set,forma);
  fprintf(f,"#%s %s xy \"%%3.0e\"\n",set,forma);
  if(v_flag==1) printf("#%s %s 1.0,1.0\n",set,size);
  fprintf(f,"#%s %s 1.0,1.0\n",set,size);
  if(v_flag==1) printf("%s %s %s\n",set,clip,one);
  fprintf(f,"%s %s %s\n",set,clip,one);
  if(v_flag==1) printf("#\n");
  fprintf(f,"#\n");
  if(label_flag) {
    if(v_flag) printf("set label\n");
    fprintf(f,"set label\n");
  }
  else {
    if(v_flag) printf("set nolabel\n");
    fprintf(f,"set nolabel\n");
  }
  
  for(i=0;i<NT;i++){
    if(xlabel != NULL) {
      if(v_flag) printf("set xlabel %s\n",xlabel[i]);
      fprintf(f,"set xlabel %s\n",xlabel[i]);
    }
    if(ylabel != NULL) {
      if(v_flag) printf("set ylabel %s\n",ylabel[i]);
      fprintf(f,"set ylabel %s\n",ylabel[i]);
    }
    typex = xscale[i];
    typey = yscale[i];

    if(!strcmp(xscale[i],"log")) {
      xm[i]=pow(10,xm[i]);
      xM[i]=pow(10,xM[i]);
    }
   


    if(point_type) {
      strcpy(pt,point_type[i]);
    }
    else strcpy(pt,"errbars");

    if(gph_name == NULL){
      if(_T!=NULL) sprintf(lixo1,"gfitn%g.gph",_T[i+1]);
      else strcpy(lixo1,"gfitn.gph");
    }
    else strcpy(lixo1,gph_name[i]);

    strcpy(Files_gph,lixo1);
    
    sprintf(gnu,"gnu%d.dat",i);
    print_data(gnu,i+1);

    if(!strncmp(typex,"log",3)){
      if(v_flag==1) printf("set logscale x\n");
      fprintf(f,"set logscale x\n");
    }
    else {
      if(v_flag==1) printf("set nlogscale x\n");
      fprintf(f,"set nologscale x\n");
    }
    if(!strncmp(typey,"log",3)){
      if(v_flag==1) {
         printf("set logscale y\n");
      }
      fprintf(f,"set logscale y\n");
    }
    else {
      if(v_flag==1) {
         printf("set nologscale y\n");
      }
      fprintf(f,"set nologscale y\n");
    }
    for(j=0;j<100;j++) gph[j]=ins[j]=lixo[j]=lixo1[j]=dlixo[j]=dlixo1[j]=0;
    if(!strcmp(typex,"logT1") || !strcmp(typex,"inv")){
      strcpy(lixo,"1/c1");
      strcpy(dlixo,"1/c1");
    }
    else {
      strcpy(lixo,"c1");
      strcpy(dlixo,"c1");
    }

    if(!strcmp(typey,"logT1") || !strcmp(typey,"inv")) {
      strcat(dlixo," 1/c2 1/c3");

      for(j=2;j<=Ma+2;j++) {
         if(!strcmp(typey,"logT1") || !strcmp(typey,"inv"))
	   sprintf(aux," 1/c%d",j);
	 else sprintf(aux," c%d",j);
         strcat(lixo,aux);
      }
      strcpy(lixo1,Files_gph);
      nchar=strlen(lixo1);
      lixo1[nchar-1]='_';   /* <--- PC version */
/*      strcat(lixo1,"_");        */
      sprintf(ins,"!cop -b2 %d %s %s %s",Ma+2,Files_gph,lixo1,lixo);
      if(v_flag==1) printf("%s\n",ins);
      fprintf(f,"%s\n",ins);
    }
    else if(!strcmp(typey,"logabs")){
      sprintf(gph,"c1");
      for(j=2;j<=Ma+2;j++) {
	sprintf(aux," abs[c%d]",j);
	strcat(gph,aux);
      }
      sprintf(ins,"!cop -b2 %d %s %s_ %s",Ma+2,Files_gph,Files_gph,gph);
      if(v_flag==1) printf("%s\n",ins);
      fprintf(f,"%s\n",ins);
      strcpy(lixo1,Files_gph);
      nchar=strlen(lixo1);
      lixo1[nchar-1]='_';  /* PC version  */
/*      strcat(lixo1,"_");  */
      strcat(dlixo," abs[c2] abs[c3]");
    }
    else {
      strcpy(lixo1,Files_gph);
      strcat(dlixo," c2 c3");
    }

    strcpy(dlixo1,gnu);
    nchar=strlen(gnu);
    gnu[nchar-1]='_';   /* PC version */

#ifdef GNUPLOT_3_5_beta_328

    if(v_flag==1) {
      printf("!cop 3 %s %s %s\n",dlixo1,gnu,dlixo);
      if(pt[0]=='p') printf("plot [%e:%e] [%e:%e] \"%s\" %s \"%%lf%%lf\" with points,",xm[i],xM[i],ym[i],yM[i],gnu,us);
      else printf("plot [%e:%e] [%e:%e] \"%s\" with errorbars,",xm[i],xM[i],ym[i],yM[i],gnu);
    }
    fprintf(f,"!cop 3 %s %s %s\n",dlixo1,gnu,dlixo);
    if(pt[0]=='p') fprintf(f,"plot [%e:%e] [%e:%e] \"%s\" %s \"%%lf%%lf\" with points,",xm[i],xM[i],ym[i],yM[i],gnu,us);
    else fprintf(f,"plot [%e:%e] [%e:%e] \"%s\"  with errorbars,",xm[i],xM[i],ym[i],yM[i],gnu);

    for(k=1;k<=Ma+1;k++) {
      for(j=0;j<100;j++) ins[j]=lixo[i]=0;
      strcpy(lixo,"\"%lf");
      for(j=0;j<k-1;j++) strcat(lixo,"%*lf");
      strcat(lixo,"%lf\"");
      sprintf(ins,"\"%s\" %s %s %s %d",lixo1,us,lixo,wl,Ma+2-k);

      if(v_flag==1) printf("%s",ins);
      fprintf(f,"%s",ins);
      if(k<Ma+1) {
        if(v_flag==1) printf(",");
        fprintf(f,",");
      }
      else {
        if(v_flag==1) printf("\n");
        fprintf(f,"\n");
      }
    }

#endif

#ifndef GNUPLOT_3_5_beta_328

    if(v_flag==1) {
      printf("!cop 3 %s %s %s\n",dlixo1,gnu,dlixo);
      if(pt[0]=='p') printf("plot [%e:%e] [%e:%e] \"%s\" %s \"%%f%%f\" with points,",xm[i],xM[i],ym[i],yM[i],gnu,us);
      else printf("plot [%e:%e] [%e:%e] \"%s\" with errorbars,",xm[i],xM[i],ym[i],yM[i],gnu);
    }
    fprintf(f,"!cop 3 %s %s %s\n",dlixo1,gnu,dlixo);
    if(pt[0]=='p') fprintf(f,"plot [%e:%e] [%e:%e] \"%s\" %s \"%%f%%f\" with points,",xm[i],xM[i],ym[i],yM[i],gnu,us);
    else fprintf(f,"plot [%e:%e] [%e:%e] \"%s\"  with errorbars,",xm[i],xM[i],ym[i],yM[i],gnu);

    for(k=1;k<=Ma+1;k++) {
      for(j=0;j<100;j++) ins[j]=lixo[i]=0;
      strcpy(lixo,"\"%f");
      for(j=0;j<k-1;j++) strcat(lixo,"%*f");
      strcat(lixo,"%f\"");
      sprintf(ins,"\"%s\" %s %s %s %d",lixo1,us,lixo,wl,Ma+2-k);

      if(v_flag==1) printf("%s",ins);
      fprintf(f,"%s",ins);
      if(k<Ma+1) {
        if(v_flag==1) printf(",");
        fprintf(f,",");
      }
      else {
        if(v_flag==1) printf("\n");
        fprintf(f,"\n");
      }
    }

#endif

    if(v_flag==1) printf("%s -1\n#\n",pausa);
    fprintf(f,"%s -1\n#\n",pausa);
  }
  fclose(f);
  if(Gnuplot == NULL) system("gnuplot gfitn.gnu");
  else {
    sprintf(lixo,"gnuplot %s",Gnuplot);
    system(lixo);
  }
}




