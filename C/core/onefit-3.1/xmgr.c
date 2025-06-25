/* $Id: xmgr.c,v 1.2 1999/10/28 15:45:15 pedros Exp $*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "xmgr.h"

void xmgr(char prog[], char format[])
     // char prog[],format[];
{
  char ins[1024],lixo[100],Files_gph[100];
  char lixo1[100],aux[100],dlixo[100],dlixo1[100],gph[100],gnu[100];
  int i,j,nchar;
  char *typex,*typey;
  char xmgr_cmd[2048];
  char xmgr_par_file[256];
  void new_line();
  char graph_type[30];
  char curv_teo[100];
  int print_data();
  int err = 1;
  
  for(i=0;i<NT;i++){
    typex = xscale[i];
    typey = yscale[i];

    if(!strcmp(xscale[i],"log")) {
      xm[i]=pow(10,xm[i]);
      xM[i]=pow(10,xM[i]);
    }


    if(gph_name == NULL){
      if(_T!=NULL) sprintf(lixo1,"gfitn%g.gph",_T[i+1]);
      else strcpy(lixo1,"gfitn.gph");
    }
    else strcpy(lixo1,gph_name[i]);

    if(XMGR_PAR_FILE == NULL){
      strcpy(xmgr_par_file,"NULL");
    }
    else strcpy(xmgr_par_file,XMGR_PAR_FILE[i]);

    strcpy(Files_gph,lixo1);
    strcpy(curv_teo,Files_gph);
    sprintf(gnu,"gnu%d.dat",i);
    print_data(gnu,i+1);

    if(!strncmp(typex,"log",3) && !strncmp(typey,"log",3)){
	strcpy(graph_type," -log xy ");
    }
    else if(!strncmp(typex,"log",3) && strncmp(typey,"log",3) != 0){
	strcpy(graph_type," -log x ");
    }
    else if(!strncmp(typey,"log",3) != 0 && !strncmp(typey,"log",3)){
	strcpy(graph_type," -log y ");
    }
    else {
      /*	strcpy(graph_type," -xy ");*/
	strcpy(graph_type,"");
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
      strcat(dlixo," 1/c2 c3/c2");

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
      sprintf(ins,"cop -b2 %d %s %s %s",Ma+2,Files_gph,lixo1,lixo);
      err = system(ins);
      strcpy(curv_teo,lixo1);
    }
    else if(!strcmp(typey,"logabs")){
      sprintf(gph,"c1");
      for(j=2;j<=Ma+2;j++) {
	sprintf(aux," abs[c%d]",j);
	strcat(gph,aux);
      }
      sprintf(ins,"cop -b2 %d %s %s_ %s",Ma+2,Files_gph,Files_gph,gph);
      err = system(ins);
      
      strcpy(curv_teo,Files_gph);

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



    if(!strcmp(typey,"logT1") || !strcmp(typey,"inv")) {
      sprintf(ins,"cop 3 %s . %s | cop 3 . %s c1 c2 c3xc2\n",dlixo1,dlixo,gnu);
      /*printf("cop 3 %s . %s | cop 3 . %s c1 c2 c3xc2\n",dlixo1,dlixo,gnu);*/
      err = system(ins);
    }
    else {
      sprintf(ins,"cop 3 %s %s %s\n",dlixo1,gnu,dlixo);
      err = system(ins);
    }
    if(!strcmp(prog,"xmgr") && !strcmp(format,"")){
      if(!strcmp(xmgr_par_file,"NULL")){
	sprintf(xmgr_cmd,"xmgr %s -nxy %s -xydy %s ",graph_type,curv_teo,gnu);
      }
      else sprintf(xmgr_cmd,"xmgr %s -nxy %s -xydy %s -param %s",graph_type,curv_teo,gnu,xmgr_par_file);
    }
    else if (!strcmp(prog,"xmgr") && strcmp(format,"")>0){
      if(!strcmp(xmgr_par_file,"NULL")){
#ifndef MacOSX
	if(display_flag) sprintf(xmgr_cmd,"grbatch %s -nxy %s -xydy %s -printfile %s.ps -device 1 -saveall %s.agr; convert -density 125x125 %s.ps %s.%s; display %s.%s",graph_type,curv_teo,gnu,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE,curv_teo,GRAPH_TYPE);
	else sprintf(xmgr_cmd,"grbatch %s -nxy %s -xydy %s -printfile %s.ps -device 1 -saveall %s.agr ; convert -density 125x125 %s.ps %s.%s",graph_type,curv_teo,gnu,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE);
#endif
#ifdef MacOSX
	if(display_flag) sprintf(xmgr_cmd,"gracebat %s -nxy %s -settypexydy %s -printfile %s.eps -device EPS -saveall %s.agr; convert -density 125x125 %s.eps %s.%s; display %s.%s",graph_type,curv_teo,gnu,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE,curv_teo,GRAPH_TYPE);
	else sprintf(xmgr_cmd,"gracebat %s -nxy %s -settypexydy %s -printfile %s.eps -device EPS -saveall %s.agr; convert -density 125x125 %s.eps %s.%s",graph_type,curv_teo,gnu,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE);
#endif
      }
#ifndef MacOSX
      else {
	if(display_flag) sprintf(xmgr_cmd,"grbatch %s -nxy %s -xydy %s -param %s -printfile %s.ps -device 1 -saveall %s.agr; convert -density 125x125 %s.ps %s.%s; display %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE,curv_teo,GRAPH_TYPE);
	else sprintf(xmgr_cmd,"grbatch  %s -nxy %s -xydy %s -param %s -printfile %s.ps -device 1 -saveall %s.agr; convert -density 125x125 %s.ps %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE);
      }
    }
#endif
#ifdef MacOSX
    else {
      if(display_flag) sprintf(xmgr_cmd,"gracebat  %s -nxy %s -settype xydy %s -param %s -printfile %s.eps -device EPS -saveall %s.agr; convert -density 125x125 %s.eps %s.%s; display %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE,curv_teo,GRAPH_TYPE);
      else sprintf(xmgr_cmd,"gracebat  %s -nxy %s -settype xydy %s -param %s -printfile %s.eps -device EPS -saveall %s.agr; convert -density 125x125 %s.eps %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE);
    }
  }
#endif
    else if (!strcmp(prog,"xmgrace")&& strcmp(format,"")==0){
      if(!strcmp(xmgr_par_file,"NULL")){
	sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -saveall %s.agr",graph_type,curv_teo,gnu,curv_teo);
      }
      else sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -param %s  -saveall %s.agr",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo);
    }
#ifndef DEBIAN9
    else if (!strcmp(prog,"xmgrace")&& strcmp(format,"")>0){
      if(!strcmp(xmgr_par_file,"NULL") && strcmp(GRAPH_TYPE,"PDF")){
	if(display_flag) sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr; display %s.%s",graph_type,curv_teo,gnu,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo,curv_teo,GRAPH_TYPE);
	else sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr",graph_type,curv_teo,gnu,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo);
      }
      else if(!strcmp(xmgr_par_file,"NULL") && !strcmp(GRAPH_TYPE,"PDF")){
	if(display_flag) sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -hdevice EPS -hardcopy -printfile %s.eps; epstopdf %s.eps  -saveall %s.agr; display %s.pdf",graph_type,curv_teo,gnu,curv_teo,curv_teo,curv_teo,curv_teo);
	else sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -hdevice EPS -hardcopy -printfile %s.eps -saveall %s.agr; epstopdf %s.eps",curv_teo,curv_teo,graph_type,curv_teo,gnu,curv_teo); 
      }
      else if(strcmp(xmgr_par_file,"NULL") && strcmp(GRAPH_TYPE,"PDF")){
	if(display_flag) sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr; display %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo,curv_teo,GRAPH_TYPE);
	else {
	  sprintf(xmgr_cmd,"xmgrace %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s  -saveall %s.agr",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo);
	}
      }
      else if(strcmp(xmgr_par_file,"NULL") && !strcmp(GRAPH_TYPE,"PDF")){
	if(display_flag) sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -param %s -hdevice EPS -hardcopy -printfile %s.eps -saveall %s.agr; epstopdf %s.eps;  display %s.pdf",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo,curv_teo,curv_teo,curv_teo);
	else sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -param %s -hdevice EPS -hardcopy -printfile %s.eps -saveall %s.agr; epstopdf %s.eps",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo,curv_teo,curv_teo);
      }
      else { 
	if(display_flag) sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr; display %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo,curv_teo,GRAPH_TYPE);
	else {
	  sprintf(xmgr_cmd,"xmgrace  %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo);
	}
      }
    }
#endif  
#ifdef DEBIAN9
    else if (!strcmp(prog,"xmgrace")&& strcmp(format,"")>0){
      if(!strcmp(xmgr_par_file,"NULL") && strcmp(GRAPH_TYPE,"PDF")){
	//	printf(":::::::::: 1  :::::::::::::\n\n");
	if(display_flag) sprintf(xmgr_cmd,"grace  %s -nxy %s -settype xydy %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr; display %s.%s",graph_type,curv_teo,gnu,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo,curv_teo,GRAPH_TYPE);
	else sprintf(xmgr_cmd,"grace  %s -nxy %s -settype xydy %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr",graph_type,curv_teo,gnu,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo);
      }
      else if(!strcmp(xmgr_par_file,"NULL") && !strcmp(GRAPH_TYPE,"PDF")){
	//	printf(":::::::::: 2 :::::::::::::\n\n");
	if(display_flag) sprintf(xmgr_cmd,"grace  %s -nxy %s -settype xydy %s -hdevice EPS -hardcopy -printfile %s.eps; epstopdf %s.eps  -saveall %s.agr; display %s.pdf",graph_type,curv_teo,gnu,curv_teo,curv_teo,curv_teo,curv_teo);
	else {
	  sprintf(xmgr_cmd,"grace  %s -nxy %s -settype xydy %s -hdevice EPS -hardcopy -printfile %s.eps -saveall %s.agr; epstopdf %s.eps",curv_teo,curv_teo,graph_type,curv_teo,gnu,curv_teo);
	}
      }
      else if(strcmp(xmgr_par_file,"NULL") && strcmp(GRAPH_TYPE,"PDF")){
	//	printf(":::::::::: 3 :::::::::::::\n\n");
	if(display_flag) sprintf(xmgr_cmd,"grace  %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr; display %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo,curv_teo,GRAPH_TYPE);
	else sprintf(xmgr_cmd,"grace %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s  -saveall %s.agr",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo);
      }
      else if(strcmp(xmgr_par_file,"NULL") && !strcmp(GRAPH_TYPE,"PDF")){
	//	printf(":::::::::: 4 ::::::::::::: %s %s\n\n",xmgr_par_file,GRAPH_TYPE);
	if(display_flag) sprintf(xmgr_cmd,"sed -i '/inf\\|nan/d' %s; grace -settype xydy %s -nxy %s -param %s -hdevice EPS -hardcopy -printfile %s.eps -saveall %s.agr; epstopdf %s.eps;  display %s.pdf",curv_teo,gnu,curv_teo,xmgr_par_file,curv_teo,curv_teo,curv_teo,curv_teo);
	else {
	  sprintf(xmgr_cmd,"sed -i '/inf\\|nan/d' %s; grace -settype xydy %s -nxy %s -param %s -hdevice EPS -hardcopy -printfile %s.eps -saveall %s.agr; epstopdf %s.eps",curv_teo,gnu,curv_teo,xmgr_par_file,curv_teo,curv_teo,curv_teo);
	}
      }
      else {
	//	printf(":::::::::: 5 :::::::::::::\n\n");
	if(display_flag) sprintf(xmgr_cmd,"grace  %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr; display %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo,curv_teo,GRAPH_TYPE);
	else sprintf(xmgr_cmd,"grace  %s -nxy %s -settype xydy %s -param %s -hdevice %s -hardcopy -printfile %s.%s -saveall %s.agr",graph_type,curv_teo,gnu,xmgr_par_file,GRAPH_TYPE,curv_teo,GRAPH_TYPE,curv_teo);
      }
    }
#endif  
  //    printf("%s\n",xmgr_cmd);
    err = system(xmgr_cmd);

  }
  if (!err) printf("system() call errror in xmgr.c, xmgr(): %d\n",err);
}

void grbatch()
{
  char ins[1024],lixo[100],Files_gph[100];
  char lixo1[100],aux[100],dlixo[100],dlixo1[100],gph[100],gnu[100];
  int i,j,nchar;
  char *typex,*typey;
  char xmgr_cmd[2048];
  char xmgr_par_file[256];
  void new_line();
  char graph_type[30];
  char curv_teo[100];
  int print_data();
  int err = 1;
  
  for(i=0;i<NT;i++){
    typex = xscale[i];
    typey = yscale[i];

    if(!strcmp(xscale[i],"log")) {
      xm[i]=pow(10,xm[i]);
      xM[i]=pow(10,xM[i]);
    }


    if(gph_name == NULL){
      if(_T!=NULL) sprintf(lixo1,"gfitn%g.gph",_T[i+1]);
      else strcpy(lixo1,"gfitn.gph");
    }
    else strcpy(lixo1,gph_name[i]);

    if(XMGR_PAR_FILE == NULL){
      strcpy(xmgr_par_file,"NULL");
    }
    else strcpy(xmgr_par_file,XMGR_PAR_FILE[i]);

    strcpy(Files_gph,lixo1);
    strcpy(curv_teo,Files_gph);
    sprintf(gnu,"gnu%d.dat",i);
    print_data(gnu,i+1);

    if(!strncmp(typex,"log",3) && !strncmp(typey,"log",3)){
	strcpy(graph_type," -log xy ");
    }
    else if(!strncmp(typex,"log",3) && strncmp(typey,"log",3) != 0){
	strcpy(graph_type," -log x ");
    }
    else if(!strncmp(typey,"log",3) != 0 && !strncmp(typey,"log",3)){
	strcpy(graph_type," -log y ");
    }
    else {
	strcpy(graph_type," -xy ");
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
      strcat(dlixo," 1/c2 c3/c2");

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
      sprintf(ins,"cop -b2 %d %s %s %s",Ma+2,Files_gph,lixo1,lixo);
      err = system(ins);
      strcpy(curv_teo,lixo1);
    }
    else if(!strcmp(typey,"logabs")){
      sprintf(gph,"c1");
      for(j=2;j<=Ma+2;j++) {
	sprintf(aux," abs[c%d]",j);
	strcat(gph,aux);
      }
      sprintf(ins,"cop -b2 %d %s %s_ %s",Ma+2,Files_gph,Files_gph,gph);
      err = system(ins);
      
      strcpy(curv_teo,Files_gph);

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



    if(!strcmp(typey,"logT1") || !strcmp(typey,"inv")) {
      sprintf(ins,"cop 3 %s . %s | cop 3 . %s c1 c2 c3xc2\n",dlixo1,dlixo,gnu);
      /*printf("cop 3 %s . %s | cop 3 . %s c1 c2 c3xc2\n",dlixo1,dlixo,gnu);*/
      err = system(ins);
    }
    else {
      sprintf(ins,"cop 3 %s %s %s\n",dlixo1,gnu,dlixo);
      err = system(ins);
    }

    if(!strcmp(xmgr_par_file,"NULL")){
      sprintf(xmgr_cmd,"grbatch  %s -nxy %s -xydy %s -printfile %s.ps -device 1 -saveall %s.agr; convert -density 125x125 %s.ps %s.%s; display %s.%s",graph_type,curv_teo,gnu,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE,curv_teo,GRAPH_TYPE);
    }
    else sprintf(xmgr_cmd,"grbatch  %s -nxy %s -xydy %s -param %s -printfile %s.ps -device 1 -saveall %s.agr; convert -density 125x125 %s.ps %s.%s; display %s.%s",graph_type,curv_teo,gnu,xmgr_par_file,curv_teo,curv_teo,curv_teo,curv_teo,GRAPH_TYPE,curv_teo,GRAPH_TYPE);
    err = system(xmgr_cmd);
    
  }
    if (!err) printf("system() call errror in xmgr.c, xmgr(): %d\n",err);
}




