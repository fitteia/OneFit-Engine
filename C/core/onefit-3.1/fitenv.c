#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfitn.h"

static char *rcsid="$Id: fitenv.c,v 1.4 1997/11/24 11:30:57 pedros Exp $";
/* The following lines will prevent `gcc' version 2.X
   from issuing an "unused variable" warning. */
#if __GNUC__ == 2
#define USE(var) static void * use_##var = (&use_##var, (void *) &var)
USE (rcsid);
#endif

typedef struct {
  char *name;
  char **value;
  int n;
} menu_entry;
  
static menu_entry menu[]={
  {"(P)ar   :",0,0},
  {"(D)ata  :",0,0},
  {"(S)etup :",0,0},
  {"(O)utput:",0,0},
  {"(F)uncao:",0,0},
  {"(L)og   :",0,0},
  {"(B)in   :",0,0},
  {"(f)it   :",0,0},
  {"(p)lot  :",0,0},
  {"(g)raphs:",0,0},
  {"(e)dit  :",0,0},
  {"(a)ctpar:",0,0},
  {"(h)elp  :",0,0},
  {"(w)rite :",0,0},
  {"(q)uit  :",0,0},
  {"e(x)it  :",0,0},
  {0,0,0}
};

static menu_entry menu_data[]={
  {"e:",0,0},
  {"i:",0,0},
  {"d:",0,0},
  {0,0,0}
};


static char editor[]="pico";

int main(argc,argv)
int argc;
char **argv;
{

  FILE *file;
  int nlinhas,*nchar_linha,i,j,k;
  char **aux,cmd[256],comm[256],**linhas,bin[10],Env_File[20];
  int begostr();
  void push_menu(),usage(),warning();

  strcpy(bin,"gfitn");
  
  
  i=0;
  while(menu[i].name != NULL){
    menu[i].value = (char **) Malloc((unsigned) 2*sizeof(char *));
    menu[i].value[0] = (char *) Malloc((unsigned) 2*sizeof(char));
    menu[i].value[0] = NULL;
    menu[i].value[1] = NULL;
    (menu[i].n) = 1;
    i++;
  }

  menu[6].value[0] = bin;
  
  FUNCAO = (char *) Malloc((unsigned) sizeof(char));
 
  if(argc==1){
    strcpy(Env_File,"fitenv.env");
    if( (file = fopen(Env_File,"r")) != NULL) {
      nchar_linha = Flc(file,&nlinhas); /* vector with n of chars per line */
      
      linhas = lines(file,nlinhas,nchar_linha);
      
      menu[0].value[0] = linhas[0]; /* parameter file */
      if(!strcmp(menu[0].value[0],"(null)")) menu[0].value[0]=NULL;
      aux = getsstring(linhas[1],&i); /* data files */

      for(j=0;j<i;j++){
	push_menu(&menu[1],aux[j]); 
      }
      menu[2].value[0] = linhas[2]; /* setup file */
      if(!strcmp(menu[2].value[0],"(null)")) menu[2].value[0]=NULL;
      menu[3].value[0] = linhas[3]; /* output file */
      if(!strcmp(menu[3].value[0],"(null)")) menu[3].value[0]=NULL;
      menu[4].value[0] = FUNCAO = linhas[4]; /* funcao */
      if(!strcmp(menu[4].value[0],"(null)")) menu[4].value[0]=FUNCAO=NULL;
      menu[5].value[0] = linhas[5]; /* log file */
      if(!strcmp(menu[5].value[0],"(null)")) menu[5].value[0]=NULL;
      if(nlinhas <= 6) {
	printf("...old version of fitenv.env! Updating it...\n");
	menu[6].value[0] = bin;
      }
      else menu[6].value[0] = linhas[6];
      fclose(file);
    }
  }
  else {
    strcpy(Env_File,argv[1]);
    if( (file = fopen(Env_File,"r")) != NULL) {
      nchar_linha = Flc(file,&nlinhas); /* vector with n of chars per line */
      
      linhas = lines(file,nlinhas,nchar_linha);
      
      menu[0].value[0] = linhas[0]; /* parameter file */
      if(!strcmp(menu[0].value[0],"(null)")) menu[0].value[0]=NULL;
      aux = getsstring(linhas[1],&i); /* data files */

      for(j=0;j<i;j++){
	push_menu(&menu[1],aux[j]); 
      }
      menu[2].value[0] = linhas[2]; /* setup file */
      if(!strcmp(menu[2].value[0],"(null)")) menu[2].value[0]=NULL;
      menu[3].value[0] = linhas[3]; /* output file */
      if(!strcmp(menu[3].value[0],"(null)")) menu[3].value[0]=NULL;
      menu[4].value[0] = FUNCAO = linhas[4]; /* funcao */
      if(!strcmp(menu[4].value[0],"(null)")) menu[4].value[0]=FUNCAO=NULL;
      menu[5].value[0] = linhas[5]; /* log file */
      if(!strcmp(menu[5].value[0],"(null)")) menu[5].value[0]=NULL;
      if(nlinhas <= 6) {
	printf("...old version of fitenv.env! Updating it...\n");
	strcpy(bin,"gfitn");
	menu[6].value[0] = bin;
      }
      else menu[6].value[0] = linhas[6];
      fclose(file);
    }
    else {
      printf("\nFITENV error: file %s not found!\n",argv[1]);
      printf("Suggestion: try to copy fitenv.env to %s!\n",argv[1]);
      exit(0);
    }
  }

  while(1){
    system("clear");
    i=0;
    printf("..................Fitting Environment (defined by %s)..................\n\n",Env_File);

    while(menu[i].name != NULL) {
      printf("%s ",menu[i].name);
      k=0;
      while(menu[i].value[k] != NULL) printf("%s ",menu[i].value[k++]);
      i++;
      printf("\n");
    }
    printf("\nfitenv>");
    gets(cmd);
    switch(cmd[0]){
    case 'P':
      if(cmd[1]!='d'){
	cmd[0]=' ';
	j = begostr(cmd,&i); 
	menu[0].value = (char **) Realloc(menu[0].value,(unsigned) 2*sizeof(char *));
	menu[0].n = 2;
	menu[0].value[1] = (char *) Malloc((unsigned) 2*sizeof(char));
	menu[0].value[1] = NULL;
	menu[0].value[0] = (char *) Realloc(menu[0].value[0], (unsigned) (i+1)*sizeof(char));
	strcpy((menu[0].value[0]),&cmd[j]);
      }
      
      else menu[0].value[0] = NULL;
      break;

    case 'D':
      cmd[0]=' ';
      if(cmd[1]=='d'){
	cmd[1]=' ';
	if(cmd[2]>=48 && cmd[2]<=57){
	  k = atoi(&cmd[2]);
	  if(k<menu[1].n){ 
	    k--;
	    for(i=k;i<menu[1].n-1;i++){
	      menu[1].value[i]=menu[1].value[i+1];
	    }
	    free((char *) menu[1].value[menu[1].n-1]);
	    menu[1].n--;
	    menu[1].value = (char **) Realloc(menu[1].value,(unsigned) (menu[1].n)*sizeof(char *));
	    menu[1].value[menu[1].n-1] = NULL;
/*
	    i=0;
	    while(menu[1].value[i] != NULL) printf("%s\n",menu[1].value[i++]);
*/
	  }
	}
	else {
	  j = begostr(cmd,&i);
	  k=0;
	  while(strncmp(&cmd[j],menu[1].value[k],i) != 0) {
	    k++;
	    if(k>menu[1].n) break;
	  }
	  if(k<menu[1].n){
	    for(i=k;i<menu[1].n-1;i++){
	      menu[1].value[i]=menu[1].value[i+1];
	    }
	    free((char *) menu[1].value[menu[1].n-1]);
	    menu[1].n--;
	    menu[1].value = (char **) Realloc(menu[1].value,(unsigned) (menu[1].n)*sizeof(char *));
	    menu[1].value[menu[1].n-1] = NULL;
/*
	    i=0;
	    while(menu[1].value[i] != NULL) printf("%s\n",menu[1].value[i++]);
*/
	  }
	}
      }
      else {
	if(swc(cmd)){
	  push_menu(&menu[1],cmd);
	}
	warning();
/*
	i=0;
	while(menu[1].value[i] != NULL) printf("%s\n",menu[1].value[i++]);
*/
      }
      break;

    case 'F':
      sprintf(cmd,"%s -l | more",menu[6].value[0]);
      system(cmd);
      gets(cmd);
      FUNCAO =(char *) Realloc(FUNCAO,(unsigned) (strlen(cmd)+1)*sizeof(char));
      strcpy(FUNCAO,cmd);
      menu[4].value = (char **) Realloc(menu[4].value,(unsigned) 2*sizeof(char *));
      menu[4].n = 2;
      menu[4].value[1] = (char *) Malloc((unsigned) 2*sizeof(char));
      menu[4].value[1] = NULL;
      menu[4].value[0] = FUNCAO;
      break;

    case 'O':
      cmd[0]=' ';
      if(cmd[1]!='d'){
	j = begostr(cmd,&i);
	menu[3].value = (char **) Realloc(menu[3].value,(unsigned) 2*sizeof(char *));
	menu[3].n = 2;
	menu[3].value[1] = (char *) Malloc((unsigned) 2*sizeof(char));
	menu[3].value[1] = NULL;
	res_output = (char *) Malloc((unsigned) (i+1)*sizeof(char));
	strcpy(res_output,&cmd[j]);
	menu[3].value[0] = res_output;
      }
      else menu[3].value[0]=NULL;
      break;

    case 'S':
      cmd[0]=' ';
      j = begostr(cmd,&i);
      menu[2].value = (char **) Realloc(menu[2].value,(unsigned) 2*sizeof(char *));
      menu[2].n = 2;
      menu[2].value[1] = (char *) Malloc((unsigned) 2*sizeof(char));
      menu[2].value[1] = NULL;

      menu[2].value[0] = (char *) Malloc((unsigned) (i+1)*sizeof(char));
      strcpy(menu[2].value[0],&cmd[j]);
      break;

    case 'L':
      cmd[0]=' ';
      if(cmd[1]!='d'){
	j = begostr(cmd,&i);
	menu[5].value = (char **) Realloc(menu[5].value,(unsigned) 2*sizeof(char *));
	menu[5].n = 2;
	menu[5].value[1] = (char *) Malloc((unsigned) 2*sizeof(char));
	menu[5].value[1] = NULL;
	
	menu[5].value[0] = (char *) Malloc((unsigned) (i+1)*sizeof(char));
	strcpy(menu[5].value[0],&cmd[j]);
      }
      else menu[5].value[0] = NULL;
      break;

    case 'B':
      cmd[0]=' ';
      if(cmd[1]!='d'){
	j = begostr(cmd,&i);
	menu[6].value = (char **) Realloc(menu[6].value,(unsigned) 2*sizeof(char *));
	menu[6].n = 2;
	menu[6].value[1] = (char *) Malloc((unsigned) 2*sizeof(char));
	menu[6].value[1] = NULL;
	
	menu[6].value[0] = (char *) Malloc((unsigned) (i+1)*sizeof(char));
	strcpy(menu[6].value[0],&cmd[j]);
      }
      else menu[6].value[0] = bin;
      break;

    case 'e':
      if(cmd[1]=='P'){
	strcpy(comm,editor);
	strcat(comm," ");
	strcat(comm,menu[0].value[0]);
	system(comm);
      }
      else if(cmd[1]=='D') {
	cmd[0]=' ';
	cmd[1]=' ';
	strcpy(comm,editor);
	if(cmd[2]>=48 && cmd[2]<=57){
	  strcat(comm," ");
	  strcat(comm,menu[1].value[atoi(&cmd[2])-1]);
	}
	else strcat(comm,cmd);
	system(comm);
      }
      else if(cmd[1]=='S'){
	strcpy(comm,editor);
	strcat(comm," ");
	strcat(comm,menu[2].value[0]);
	system(comm);
	warning();
      }
      else if(cmd[1]=='O'){
	strcpy(comm,editor);
	strcat(comm," ");
	strcat(comm,menu[3].value[0]);
	system(comm);
      }
      else if(cmd[1]=='L'){
	strcpy(comm,editor);
	strcat(comm," ");
	strcat(comm,menu[5].value[0]);
	system(comm);
      }
      break;

    case 'f':
      switch(cmd[1]){
      case '&':
	sprintf(comm,"cp %s fitenv.stp\0",menu[2].value[0]);
	system(comm);
	file = openf("fitenv.stp","a");
	if(menu[4].value[0]!=NULL) fprintf(file,"--Function=%s\n",FUNCAO);
	if(menu[0].value[0]!=NULL) fprintf(file,"--Parameter=%s\n",menu[0].value[0]);
	fprintf(file,"--fit\n-np\n");
	if(menu[3].value[0]!=NULL) fprintf(file,"-o%s\n",menu[3].value[0]);
	fclose(file);
	
/*	sprintf(comm,"%s -@fitenv.stp",menu[6].value[0]); */
	sprintf(comm,"time %s -@fitenv.stp",menu[6].value[0]);
	for(i=1;i<=menu[1].n-1;i++) {
	  strcat(comm," ");
	  strcat(comm,menu[1].value[i-1]);
	}
	strcat(comm," ");
	
#ifndef ALF1
	if(menu[0].value[0]!=NULL) {
	  if(menu[5].value[0] == NULL) {
	    strcat(comm,"<");
	    strcat(comm,menu[0].value[0]);
	  }
	  else {
	    strcat(comm,"<");
	    strcat(comm,menu[0].value[0]);
	    strcat(comm," >");
	    strcat(comm,menu[5].value[0]);
	  }
	}
	else {
	  if(menu[5].value[0] != NULL) {
	    strcat(comm," >");
	    strcat(comm,menu[5].value[0]);
	  }
	}
#endif
#ifdef ALF1
	setenv("FORT5",menu[0].value[0],1);
	if(menu[5].value[0]!=NULL) setenv("FORT6",menu[5].value[0],1);
	else setenv("FORT6","gfitn.log",1);
#endif
/*	strcat(comm," &"); */
	strcat(comm," 2>time.log &");
	printf("%s\n",comm);
	system(comm);
	printf("\n ...press <return> to continue...");
	gets(cmd);
	break;

      default:
	sprintf(comm,"cp %s fitenv.stp\0",menu[2].value[0]);
	system(comm);
	file = openf("fitenv.stp","a");
	if(menu[4].value[0]!=NULL) fprintf(file,"--Function=%s\n",FUNCAO);
	if(menu[0].value[0]!=NULL) fprintf(file,"--Parameter=%s\n",menu[0].value[0]);
	fprintf(file,"--fit\n-np\n");
	if(menu[3].value[0]!=NULL) fprintf(file,"-o%s\n",menu[3].value[0]);
	fclose(file);
	
	sprintf(comm,"%s -@fitenv.stp",menu[6].value[0]);
	for(i=1;i<=menu[1].n-1;i++) {
	  strcat(comm," ");
	  strcat(comm,menu[1].value[i-1]);
	}
	strcat(comm," ");
	
#ifndef ALF1
	if(menu[0].value[0]!=NULL) {
	  if(menu[5].value[0] == NULL) {
	    strcat(comm,"<");
	    strcat(comm,menu[0].value[0]);
	  }
	  else {
	    strcat(comm,"<");
	    strcat(comm,menu[0].value[0]);
	    strcat(comm," >");
	    strcat(comm,menu[5].value[0]);
	  }
	}
	else {
	  if(menu[5].value[0] != NULL) {
	    strcat(comm," >");
	    strcat(comm,menu[5].value[0]);
	  }
	}
#endif
#ifdef ALF1
	setenv("FORT5",menu[0].value[0],1);
	if(menu[5].value[0]!=NULL) setenv("FORT6",menu[5].value[0],1);
	else setenv("FORT6","gfitn.log",1);
#endif
	printf("%s\n",comm);
	system(comm);
	printf("\n ...press <return> to continue...");
	gets(cmd);
	break;
      }
      break;
    case 'p':
      sprintf(comm,"cp %s fitenv.stp\0",menu[2].value[0]);
      system(comm);
      file = openf("fitenv.stp","a");
      if(menu[4].value[0]!=NULL) fprintf(file,"--Function=%s\n",FUNCAO);
      fprintf(file,"-nf\n-pg\n");
      if(menu[3].value[0]!=NULL) fprintf(file,"-o%s\n",menu[3].value[0]);
      fclose(file);

      sprintf(comm,"%s -@fitenv.stp\0",menu[6].value[0]);

      for(i=1;i<=menu[1].n-1;i++) {
	strcat(comm," ");
	strcat(comm,menu[1].value[i-1]);
      }
      printf("%s\n",comm);
      system(comm);
      break;

    case 'g':
      sprintf(comm,"cp %s fitenv.stp\0",menu[2].value[0]);
      system(comm);
      file = openf("fitenv.stp","a");
      if(menu[4].value[0]!=NULL) fprintf(file,"--Function=%s\n",FUNCAO);
      fprintf(file,"-nfp\n-g\n");
      if(menu[3].value[0]!=NULL) fprintf(file,"-o%s\n",menu[3].value[0]);
      fclose(file);

      sprintf(comm,"%s -@fitenv.stp\0",menu[6].value[0]);

      for(i=1;i<=menu[1].n-1;i++) {
	strcat(comm," ");
	strcat(comm,menu[1].value[i-1]);
      }
      printf("%s\n",comm);
      system(comm);
      break;
      
    case '!':
      system(&cmd[1]);
      printf("\n ...press <return> to continue...");
      gets(cmd);
      break;

    case 'a':
      sprintf(comm,"actpar %s %s\0",menu[3].value[0],menu[0].value[0]);
      system(comm);
      break;

    case 'h':
      usage();
      gets(cmd);
      break;

    case 'q':
      exit(0);
      break;

    case 'x':
      file = openf(Env_File,"w");
      fprintf(file,"%s\n",menu[0].value[0]); /* parameter file */
      
      for(j=0;j<menu[1].n-1;j++){
	fprintf(file,"%s ",menu[1].value[j]); 
      }
      fprintf(file,"\n");
      fprintf(file,"%s\n",menu[2].value[0]); /* setup file */
      fprintf(file,"%s\n",menu[3].value[0]); /* output file */
      fprintf(file,"%s\n",menu[4].value[0]); /* funcao */
      fprintf(file,"%s\n",menu[5].value[0]); /* log file */
      fprintf(file,"%s\n",menu[6].value[0]); /* bin file */
      fclose(file);
      exit(0);
      break;

    case 'w':
      cmd[0]=' ';
      j = begostr(cmd,&i);
      strcpy(Env_File,&cmd[j]);
      file = openf(Env_File,"w");
      fprintf(file,"%s\n",menu[0].value[0]); /* parameter file */
      
      for(j=0;j<menu[1].n-1;j++){
	fprintf(file,"%s ",menu[1].value[j]); 
      }
      fprintf(file,"\n");
      fprintf(file,"%s\n",menu[2].value[0]); /* setup file */
      fprintf(file,"%s\n",menu[3].value[0]); /* output file */
      fprintf(file,"%s\n",menu[4].value[0]); /* funcao */
      fprintf(file,"%s\n",menu[5].value[0]); /* log file */
      fprintf(file,"%s\n",menu[6].value[0]); /* bin file */
      fclose(file);
      break;
      
    default:
      break;
    }
  }

  exit(0);
  
}

int begostr(str,size)
char *str;
int *size;
{
  int i,j;

  for(i=0;i<=strlen(str);i++)
    if(str[i] !=' ') break;

  *size = strlen(str)-i;
  return i;
}

void push_menu(menu,str)
menu_entry *menu;
char *str;
{
  int i,j,n;

  menu->value = (char **) Realloc(menu->value,(unsigned) (++menu->n)*sizeof(char *));
  menu->value[menu->n-1] = (char *) Malloc((unsigned) 2*sizeof(char));
  menu->value[menu->n-1] = NULL;
  menu->value[menu->n-2] = (char *) Realloc(menu->value[menu->n-2], (unsigned) 80*sizeof(char));
  j = begostr(str,&i); 
  strcpy((menu->value[menu->n-2]),&str[j]);
}

  
void usage()
{
  printf("Welcome to FITENV, a user defined fitting environment\n");
  printf("\n");
  printf("usage: fitenv [environment file]\n\n");
  printf("P  <parameter file name>\n");
  printf("Pd <to exclude parameter file from fitting process>\n");
  printf("D  <data file name>\n");
  printf("Dd[data file order number] [file name] <to exclude file from fitting process>\n");
  printf("S  <set up file name wich contains some default options of gfitn>\n");
  printf("O  <output file name (if none gfitn.out will be used)>\n");
  printf("Od <to reset output default file name>\n");
  printf("F  <fitting function>\n");
  printf("L  <log file name (if none screen will be used)>\n");
  printf("Ld <to reset log to screen>\n");
  printf("B  <bin to file to use with fitenv (default is gfitn)>\n");
  printf("Bd <to reset bin file to gfitn>\n");
  printf("f[&]  <to fit (use & to fit in backgroud and save CPU used time in time.log)>\n");
  printf("p  <to plot>\n");
  printf("g  <to send graphs to output device>\n");
  printf("eP <to edit parameters file>\n");
  printf("eD[data file order number] [file name] <to edit file>\n");
  printf("eS <to edit setup file>\n");
  printf("eO <to edit output file>\n");
  printf("eL <to edit log file>\n");
  printf("a  <update fit starting values in parameter file with the last fit output>\n");
  printf("h  <to get help on this menu>\n");
  printf("w  <to write environment file name>\n");
  printf("q  <to quit without saving environment>\n");
  printf("x  <to exit and save environment>\n");
  printf("!<command (to execute linux shell command)>\n");

  printf("\n ...press <return> to continue...");
}

void warning()
{
  char answer[2];
  printf("\n\nCheck if the number of arguments in each option match the number of data files\n");
  printf("FITENV dosen't control this. You have to do it yourself\n");
  printf("\n ...press <return> to continue...");
  gets(answer);
}




