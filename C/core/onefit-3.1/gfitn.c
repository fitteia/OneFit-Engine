/* $Id: gfitn.c,v 1.4 1999/10/28 15:45:10 pedros Exp $ */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef MacOSX
     #include <malloc.h>
#endif
#include "gfitn.h"
#include <stdlib.h>
#include "readdat.h"
#include "xmgr.h"
#include "gfit_out.h"
#include "gfitoutp.h"
#include "gchi2_vn.h"
#include "getopt1.h"

#ifdef WITH_PERL
	#include <EXTERN.h>
	#include <perl.h>
	static PerlInterpreter *my_perl;
#endif
/*static char *PERL_FILE[]={"","gfitn.pl"};*/
static char **PERL_FILE;

int main(int argc, char **argv,char **env)
// int argc;
// char **argv;
// char **env;
{
  int i,m;
  double out,*chis;

  PROGNAME = argv[0];

  fprintf(stderr,"OneFit, version %s.\n",version_number);
  fprintf(stderr,"Thank you for using OneFit. Every constructive criticism is welcome.\n");
  fprintf(stderr,"\nOneFit is free software with ABSOLUTLY NO WARRANTY.\n\n");
  fprintf(stderr,"It provides a way of performing non-linear data fitting of user\n");
  fprintf(stderr,"defined functions compiled with the OneFit kernel program.\n\n");
#ifdef WITH_PERL
  fprintf(stderr,"OneFit has, since version GFITN 5.0.0, an embeded PERL interpreter\n");
  fprintf(stderr,"which makes it easier to program and test fitting functions.\n");
  fprintf(stderr,"However, OneFit is slowed by a factor 100 when running PERL functions.\n\n");
#endif

  fprintf(stderr,"OneFit accesses non-linear fitting parameters through their names\nindependently of their order in the parameter file.\nYou may find it usefull to use X1fit (OneFit GUI) to manage OneFit\n\n");
  fprintf(stderr,"Copyright (C) 1990-2025, Pedro Jos%c Oliveira Sebasti%co\n\n",233,227);
/*  fprintf(stderr,"\n(The idea used in the first version of this program");
  fprintf(stderr,"\n was the result of a suggestion of Carlos Cruz");
  fprintf(stderr,"\n Thank you Carlos for that challenge)\n\n");
*/
  if(argc == 1) {
    printf("Try gfitn -h for help\n");
    exit(0);
  }
    

  initialize();
  user_initialize();

  read_arguments(argc,argv);

  if(list_flag) exit(0);

#ifdef WITH_PERL
  if(PERL_FILE){
    my_perl = perl_alloc();
    perl_construct(my_perl);
    perl_parse(my_perl, NULL, 2, PERL_FILE, NULL);
    free_cmatrix(PERL_FILE,0,1,0,100);
  }
#endif

  NT = argc - optind;
/*  printf("argc=%d, NT=%d\n",argc,NT);*/
  dfiles = (char **) Malloc ((unsigned) NT*sizeof(char *));

  decode_funcao(funcao);

/*  for(i=0;i<Ma;i++) {
    for(m=0;m<NT;m++) printf("%s=%g ",AA_names[i][m],AA[i+1][m+1]);
    printf(" %g\n",(*function[i])(2.0));
  }
  for(i=1;i<=Ma;i++) puti(Lista[i]);
*/
  Ndata = ivector(1,NT);
  _T     = dvector(1,NT);

  VData = (double ***) Malloc((unsigned) NT*sizeof(double **));
  
/*  puti(NT); */
  printf("Fitting %d data curves\n",NT);

  i=0;
  while(optind < argc){
    dfiles[i++]=argv[optind];
    read_data(argv[optind]);

    printf("Data file: %s",argv[optind]);
    putchar('\n');
    optind++;
  }

  if(!xscale_flag){
    xscale = (char **) Malloc((unsigned) NT*sizeof(char *));
    for(i=0;i<NT;i++) {
      xscale[i] = (char *) Malloc((unsigned) 4*sizeof(char));
      strcpy(xscale[i],"lin");
    }
  }
  if(!yscale_flag){
    yscale = (char **) Malloc((unsigned) NT*sizeof(char *));
    for(i=0;i<NT;i++) {
      yscale[i] = (char *) Malloc((unsigned) 4*sizeof(char));
      strcpy(yscale[i],"lin");
    }
  }

  if(autox_flag){
    if(xm!=NULL) free((char*) xm);
    if(xM!=NULL) free((char*) xM);

    xm = dvector(0,NT-1);
    xM = dvector(0,NT-1);

    for(i=0;i<NT;i++){
      xm[i]=VData[i][1][1];
      xM[i]=VData[i][1][1];
      for(m=1;m<=Ndata[i+1];m++) {
	xm[i] = myMIN(xm[i],VData[i][m][1]);
	xM[i] = myMAX(xM[i],VData[i][m][1]);
      }
      if(!strcmp(xscale[i],"log")) {
	xm[i]=log10(xm[i]);
	xM[i]=log10(xM[i]);
      }
    }
  }
  else {
    for(i=0;i<NT;i++){
      if(!strcmp(xscale[i],"log")) {
	xm[i]=log10(xm[i]);
	xM[i]=log10(xM[i]);
      }
    }
  }

  if(autoy_flag){
    if(!ym) free((char*) ym);
    if(!yM) free((char*) yM);
    
    ym = dvector(0,NT-1);
    yM = dvector(0,NT-1);
    
    for(i=0;i<NT;i++){
      ym[i]=VData[i][1][2];
      yM[i]=VData[i][1][2];
      for(m=1;m<=Ndata[i+1];m++) {
	ym[i] = myMIN(ym[i],VData[i][m][2]);
	yM[i] = myMAX(yM[i],VData[i][m][2]);
      }
/*      putd(ym[i]); */
/*      putd(yM[i]); */
    }
  }
  
  if(num_gph_points_flag){
    num_gph_points = ivector(0,NT-1);
    for(i=0;i<NT;i++) num_gph_points[i]=Ndata[i+1];
  }

  
/**/
  if(nolinpar_flag){
      if(parfile == NULL) usage();
      else {
	  ParNames = ReadParNames(parfile);
	  ParFreeF(parfile);
      }      
  }

  if(fit_flag){
      if(nolinpar_flag){
	if(parfile == NULL) usage();
	else {
	  /*
	    ParNames = ReadParNames(parfile);
	    ParFreeF(parfile);
	  */
	  
	  printf("Adjusting FPar=%d non-linear parameters\n",FPar);
	  for(i=1;i<=FPar;i++) printf("Par. no. %d is free\n",ParFreeVector[i]);
	  
#ifdef LINUX
	  minuit_(fcn,0);
#endif
#ifndef LINUX
	  minuit_MSDOS(fcn,0);
#endif
	  free_ivector(ParFreeVector,1,FPar);
	}
      }
      else {
	FPar = 0;
	chis = dvector(1,NT);
	out = gchi2_vn(NULL,chis);
	putd(out);
	gfit_outpg(NULL);
	gfit_outgraf(NULL);
	free_dvector(chis,1,NT);
      }
  }
  
/**/  
  if(plot_flag) plotfitn();

  /*    printf("plot_flag=%d xmgr_flag =%d xmgrace_flag =%d graphic_flag=%d grbatch_flag=%d GRAPH_TYPE=%s\n",plot_flag,xmgr_flag,xmgrace_flag,graphic_flag,grbatch_flag,GRAPH_TYPE);*/

  if(graphic_flag && xmgr_flag == 0 && grbatch_flag == 0 && xmgrace_flag == 0) mygnus();
  else if(graphic_flag && xmgr_flag == 1 && grbatch_flag == 0) xmgr("xmgr","");
  else if(graphic_flag && xmgr_flag == 1 && grbatch_flag == 1) xmgr("xmgr",GRAPH_TYPE);
  else if(graphic_flag && xmgrace_flag == 1 && grbatch_flag == 0) xmgr("xmgrace","");
  else if(graphic_flag && xmgrace_flag == 1 && grbatch_flag == 1) xmgr("xmgrace",GRAPH_TYPE);
	  

#ifdef WITH_PERL
  perl_destruct(my_perl);
  perl_free(my_perl);
#endif
  exit(0);
}


void read_arguments(int argc, char *argv[])
// int argc;
// char *argv[];
{
    int getopt_long();
    int i,j,n,m;
    int optind_aux=0;
    char *optarg_aux=NULL;
    int c,nlinhas,*nchar_linha;
    int option_index=0;
    char **opt;
    FILE *optfile;
    
    while(1){
	c = getopt_long(argc,argv,short_options,long_options,&option_index);

	if(c == -1) break;
	
	switch(c){
	    
	case 0:
	    break;
	    
	case 1:
	    device = optarg;
	    /*      printf("terminal =  %s \n",device); */
	    break;
	    
	case 2:
	    outdev = optarg;
	    outdev_flag=1;                         /* output to file */
	    /*      printf("device outfile: %s ",outdev); */
	    break;
	    
	case 3:
	    if(optarg){
		xlabel = getsstring(optarg,&m);
		if(first_time) {
		    NT = m;
		    first_time=NO;
		}
		else if(m != NT) nrerror("missing arguments in xlabel");
		/*	while(--m>=0) puts(xlabel[m]);*/
	    }
	    break;
	    
	case 4:
	    if(optarg){
		ylabel = getsstring(optarg,&m);
		if(first_time) {
		    NT = m;
		    first_time=NO;
		}
		else if(m != NT) nrerror("missing arguments in ylabel");
		/*	while(--m>=0) puts(ylabel[m]);*/
	    }
	    break;
	    
	case 5:
	    xscale_flag=1;
	    if(optarg){
		xscale = getsstring(optarg,&m);
		if(first_time) {
		    NT = m;
		    first_time=NO;
		}
		else if(m != NT) nrerror("missing arguments in xscale");
		/*	while(--m>=0) puts(xscale[m]); */
	    }
	    break;
	    
	case 6:
	    yscale_flag=1;
	    if(optarg){
		yscale = getsstring(optarg,&m);
		if(first_time) {
		    NT = m;
		    first_time=NO;
		}
		else if(m != NT) nrerror("missing arguments in yscale");
		/*	while(--m>=0) puts(yscale[m]);*/
	    }
	    break;
	    
	case 7:
	    if(optarg){
		point_type = getsstring(optarg,&m);
		if(first_time) {
		    NT = m;
		    first_time=NO;
		}
		else if(m != NT) nrerror("missing arguments in point_type");
		/*	while(--m>=0) puts(point_type[m]);*/
	    }
	    break;
	    
	case 8:
	    if(optarg){
		gph_name = getsstring(optarg,&m);
		if(first_time) {
		    NT = m;
		    first_time=NO;
		}
		else if(m != NT) nrerror("missing arguments in gph_name");
		/*	while(--m>=0) puts(gph_name[m]);*/
	    }
	    break;
	    
	case 9:
	    num_gph_points_flag = 0;
	    if(optarg){
		char **a;
		a = getsstring(optarg,&m);
		if(first_time) {
		    NT = m;
		    first_time=NO;
		}
		else if(m != NT) nrerror("missing arguments in num_gph_points");
		num_gph_points = ivector(0,m-1);
		while(--m>=0) {
		    num_gph_points[m]=atoi(a[m]);
		    /*	  printf("%d\n",num_gph_points[m]); */
		    free((char *) a[m]);
		}
		
		free((char *) a);
	    }
	    break;

    case 10:
      if(optarg) funcao=optarg;
      m = strlen(funcao);
      FUNCAO = (char *) Malloc((unsigned) (m+1)*sizeof(char));
      strcpy(FUNCAO,funcao);
      break;

    case 11:
      if(optarg) parfile = optarg;
/*      printf("Parfile =  %s \n",parfile); */
      break;

    case 12:
      autox_flag =0;
      if(optarg){
	char **a;
	a = getsstring(optarg,&m);
	if(first_time) {
	  NT = m;
	  first_time=NO;
	}
	else if(m != NT) nrerror("missing arguments in xm option");
	xm = dvector(0,m-1);
	while(--m>=0) {
	  xm[m]=atof(a[m]);
/*	  printf("xm=%g\n",xm[m]); */
	  free((char *) a[m]);
	}

	free((char *) a);
      }
      break;

    case 13:
      autox_flag =0;
      if(optarg){
	char **a;
	a = getsstring(optarg,&m);
	if(first_time) {
	  NT = m;
	  first_time=NO;
	}
	else if(m != NT) nrerror("missing arguments in xM option");
	xM = dvector(0,m-1);
	while(--m>=0) {
	  xM[m]=atof(a[m]);
/*	  printf("xM=%g\n",xM[m]); */
	  free((char *) a[m]);
	}

	free((char *) a);
      }
      break;

    case 14:
      autoy_flag =0;
      if(optarg){
	char **a;
	a = getsstring(optarg,&m);
	if(first_time) {
	  NT = m;
	  first_time=NO;
	}
	else if(m != NT) nrerror("missing arguments in ym option");
	ym = dvector(0,m-1);
	while(--m>=0) {
	  ym[m]=atof(a[m]);
/*	  printf("ym=%g\n",ym[m]); */
	  free((char *) a[m]);
	}

	free((char *) a);
      }
      break;

    case 15:
      autoy_flag =0;
      if(optarg){
	char **a;
	a = getsstring(optarg,&m);
	if(first_time) {
	  NT = m;
	  first_time=NO;
	}
	else if(m != NT) nrerror("missing arguments in yM option");
	yM = dvector(0,m-1);
	while(--m>=0) {
	  yM[m]=atof(a[m]);
/*	  printf("yM=%g\n",yM[m]); */
	  free((char *) a[m]);
	}

	free((char *) a);
      }
      break;

    case 16:
      Gnuplot=optarg;
      break;

    case 17:
      xy_flag  = 1;
      xyt_flag = xyz_flag = 0;
      break;

    case 18:
      xyz_flag  = 1;
      xy_flag = xyt_flag = 0;
      break;

    case 19:
      xyt_flag  = 1;
      xyz_flag = xy_flag = 0;
      break;

    case 20:
      xmgr_flag  = 1;
      break;

    case 21:
      if(optarg){
	XMGR_PAR_FILE = getsstring(optarg,&m);
	if(first_time) {
	  NT = m;
	  first_time=NO;
	}
	else if(m != NT) nrerror("missing arguments in XMGR_PAR_FILE");
/*	while(--m>=0) puts(gph_name[m]);*/
      }
      break;

    case 22:
	PERL_FILE=cmatrix(0,1,0,100);
	strcpy(PERL_FILE[0],"");
	strcpy(PERL_FILE[1],optarg);
	break;

    case 23:
      grbatch_flag  = 1;
      GRAPH_TYPE=cvector(0,10);
      strcpy(GRAPH_TYPE,optarg);
      break;

    case 24:
      xmgrace_flag  = 1;
      break;

    case 25:
      display_flag  = 1;
      break;

    case 'D':
      display_flag = 1;
      break;

    case 'd':
      device = optarg;
/*      printf("terminal =  %s \n",device); */
      break;

    case 'e':
      errors_flag = 1;
      break;

    case 'f':
      fit_flag=1;
      break;

    case 'g':
      graphic_flag=1;
      break;

    case 'h':
      usage();
      break;

    case 'l':
      list_flag=1;
      fprintf(stderr,"listing gfitn pre-defined functions...\n");
      initialize();
      user_initialize();
      fprintf(stderr,"...finished! Rerun to fit.\n");
      break;

    case 'n':
      for(i=0;i<=strlen(optarg);i++){
	switch(optarg[i]){
	case 'p':
	  plot_flag=0;
	  break;
	  
	case 'f':
	  fit_flag=0;
	  break;

	case 'g':
	  graphic_flag=0;
	  break;

	case 'D':
	  display_flag=0;
	  break;
	}
      }
      break;

    case 'o':
      res_output = optarg;
/*      printf("option -o with value %s\n",res_output); */
      break;

    case 'p':
      plot_flag=1;
      break;

    case 't':
      xyt_flag = 1;
      xy_flag = xyz_flag = 0;
      break;

    case 'z':
      xyz_flag=1;
      xy_flag = xyt_flag =0;
      break;

    case 'G':
      Gnuplot=optarg;
      break;

    case 'F':
      if(optarg) funcao=optarg;
      m = strlen(funcao);
      FUNCAO = (char *) Malloc((unsigned) (m+1)*sizeof(char));
      strcpy(FUNCAO,funcao);
      break;

    case 'O':
      outdev = optarg;
      outdev_flag=1;                         /* output to file */
/*      printf("device outfile: %s ",outdev); */
      break;

    case 'P':
      parfile = optarg;
/*      printf("option -P with value %s\n",parfile); */
      break;

    case'@':
      optfile = openf(optarg,"r");
      optind_aux=optind;
      optarg_aux=optarg;
      optind=0;
      optarg = NULL;
      nchar_linha=Flc(optfile,&nlinhas); /* vector with n of chars per line */
      opt = lines(optfile,nlinhas,nchar_linha);
      n = nlinhas+2;
      myoptions = (char **) Malloc((unsigned) n*sizeof(char *));
      myoptions[0]=argv[0];
      for(j=1,i=1; j<=nlinhas;j++) {
	myoptions[i]=purges(opt[j-1]);
/* linhas que descomentadas permitem ter mais do que uma opcao por linha mas
   nao varios argumentos para uma opcao
	aux = getsstring(opt[j-1],&m);
	if(m>1) {
	  l = i;
	  i += m;
	  n += m-1;
	  myoptions = (char **) Realloc(myoptions,(unsigned) n*sizeof(char *));
	  while(--m>=0) myoptions[l+m] = aux[m];
	}
	else i++;
*/
	i++;
      }
      myoptions[n-1]=0;

/*      for(i=0; i<=n-1;i++) printf("%d %s\n",i,myoptions[i]);
      printf("%d\n",n-1);
*/
      read_arguments(n-1,myoptions);
      fclose(optfile);
      optarg = optarg_aux;
      optind = optind_aux;
/*      printf("optind=%d\n",optind);*/
      break;

    default:
      abort();
    }
  }
}
  
void decode_funcao(char *optarg)
{
  char **a,**b,**c,**fnames;
  int i,j,k,m,n,fn,sucess=NO;

  m = strlen(optarg);
  for(i=0;i<m;i++) if(optarg[i] == ' ') optarg[i]=':';
  for(i=0;i<m;i++) if(optarg[i] == '+') optarg[i]=' ';
  a = getsstring(optarg,&Ma);
  function =(double (**)()) Malloc((unsigned) Ma*sizeof(double (*)()));
  AA_names =(char ***) Malloc((unsigned) Ma*sizeof(char **));
  Func_args=(char ***) Malloc((unsigned) Ma*sizeof(char **));
  Func_nargs=ivector(0,Ma-1);
  for(i=0;i<Ma;i++) AA_names[i]=(char **) Malloc((unsigned) NT*sizeof(char *));

  Lista = ivector(1,Ma);
  Covar = dmatrix(1,Ma,1,Ma);
  AA    = dmatrix(1,Ma,1,NT);

  Func_names = cmatrix(0,Ma-1,0,50);


  for(i=0,k=1;i<Ma;i++) {
    m = strlen(a[i]);

    for(j=0;j<m;j++) if(a[i][j] == '*') a[i][j]=' ';
    b = getsstring(a[i],&m);
    for(j=0;j<strlen(b[m-1]);j++) if(b[m-1][j]=='(' || b[m-1][j]==')' ) b[m-1][j]=' ';
    printf("%s\n",b[m-1]);
    fnames=getsstring(b[m-1],&fn);
    for(j=0;j<strlen(fnames[fn-1]);j++) if(fnames[fn-1][j]==':') fnames[fn-1][j]=' ';
    if(fn==1){
	Func_args[i]=NULL;
	fnames[fn-1]=purges(fnames[fn-1]);
	for(j=0;j<Nfuncoes;j++){
	    if(!strcmp(fnames[fn-1],foptions[j].name)){
		strcpy(Func_names[i],fnames[fn-1]);
		printf("gfitn:Cfunc=%s\n",Func_names[i]);
		function[i] = foptions[j].ptr;
		if(foptions[j].flag) nolinpar_flag=1;
		sucess = YES;
		break;
	    }
	    else sucess = NO;
	}
	if(!strncmp(fnames[fn-1],"PerlLF",6)){
	    function[i] = PerlLF;
	    strcpy(Func_names[i],fnames[fn-1]);
	    printf("gfitn:PerlLF=%s\n",Func_names[i]);
	    nolinpar_flag=0;
	    sucess = YES;
	}
	else if(!strncmp(fnames[fn-1],"PerlNLF",7)){
	    function[i] = PerlNLF;
	    strcpy(Func_names[i],fnames[fn-1]);
	    printf("gfitn:PerlNLF%s\n",Func_names[i]);
	    nolinpar_flag=1;
	    sucess = YES;
	}
    }
    else {
	fnames[fn-2]=purges(fnames[fn-2]);
	fnames[fn-1]=purges(fnames[fn-1]);
	printf("Gfitn1:decode fnames=%s with args",fnames[fn-2]);
	for(j=0;j<strlen(fnames[fn-1]);j++) if(fnames[fn-1][j]==',') fnames[fn-1][j]=' ';
	Func_args[i]=getsstring(fnames[fn-1],&Func_nargs[i]);
	for(j=0;j<Func_nargs[i];j++)
	      printf(" arg=%s ",Func_args[i][j]);
	printf("\n");
	for(j=0;j<Nfuncoes;j++){
	    if(!strcmp(fnames[fn-2],foptions[j].name)){
		strcpy(Func_names[i],fnames[fn-2]);
		printf("gfitn:Cfunc=%s\n",Func_names[i]);
		function[i] = foptions[j].ptr;
		if(foptions[j].flag) nolinpar_flag=1;
		sucess = YES;
		break;
	    }
	    else sucess = NO;
	}
	if(!strncmp(fnames[fn-2],"PerlLF",6)){
	    function[i] = PerlLF;
	    strcpy(Func_names[i],fnames[fn-2]);
	    printf("gfitn:PerlLF=%s\n",Func_names[i]);
	    nolinpar_flag=0;
	    sucess = YES;
	}
	else if(!strncmp(fnames[fn-2],"PerlNLF",7)){
	    function[i] = PerlNLF;
	    strcpy(Func_names[i],fnames[fn-2]);
	    printf("gfitn:PerlNLF%s\n",Func_names[i]);
	    nolinpar_flag=1;
	    sucess = YES;
	}
    }
    if(sucess == NO) gfitn_error("Decoding function...\nFunction not programed:",b[m-1]);

    /*    printf("m=%d\n",m);*/
 
    if(m==1){
      for(j=0;j<NT;j++) {
	AA_names[i][j] = (char *) Malloc((unsigned) 3*sizeof(char));
	strcpy(AA_names[i][j],"AA");
	/*	printf("read AA_names[%d][%d]=%s\n",i,j,AA_names[i][j]);*/
	AA[i+1][j+1]=1.0;
      }
    }
    else if(m>1){
      for(j=0;j<strlen(b[m-2]);j++) if(b[m-2][j] == '[' || b[m-2][j]==']' || b[m-2][j]==':') b[m-2][j]=' ';

      c = getsstring(b[m-2],&n);
      if(n==1){
	c[0]=purges(c[0]);
	if(c[0][0]> 57) {
	  for(j=0;j<NT;j++) {
	    AA_names[i][j]=c[0];
	    AA[i+1][j+1]=1.0;
	    /*	    printf("read AA_names[%d][%d]=%s\n",i,j,AA_names[i][j]);*/
	  }
	  Lista[k++]=i+1;
	  Mfit++;
	}
	else {
	  for(j=0;j<NT;j++) {
	    AA_names[i][j] = (char *) Malloc((unsigned) 3*sizeof(char));
	    strcpy(AA_names[i][j],"AA");
	    /*	    printf("read AA_names[%d][%d]=%s\n",i,j,AA_names[i][j]);*/
	    AA[i+1][j+1]=atof(c[0]);
	  }
	}

      } 
      else if(n==NT){
	for(j=0;j<n;j++){
	  c[j]=purges(c[j]);
	  if(c[j][0]> 57) {
	    AA_names[i][j]=c[j];
	    AA[i+1][j+1]=1.0;
	    printf("%s\n",c[j]);
	    /*	    nrerror("ERROR: (decode_funcao) literal string insted of numeric value");*/
	  }
	  else {
	    AA_names[i][j] = (char *) Malloc((unsigned) 3*sizeof(char));
	    strcpy(AA_names[i][j],"AA");
	    /*	    printf("read AA_names[%d][%d]=%s\n",i,j,AA_names[i][j]);*/
	    AA[i+1][j+1]=atof(c[j]);
	  }
	}
      }
      else nrerror("ERROR: (decode_funcao) missing value in funcion definition");
    }
    /*    printf("AA_names=%s\n",AA_names[i][0]);*/
  }
  while(k <= Ma) Lista[k++]=0;
}

double perl_func_call(char s[],char *spointer[])
{
    double a=0;
#ifdef WITH_PERL
    dSP;
    ENTER;
    SAVETMPS;
    PUSHMARK(SP);
    perl_call_argv(s,G_SCALAR,spointer);
    SPAGAIN;
    a=POPn;
    PUTBACK;
    FREETMPS;
    LEAVE;
#endif
    return a;
}

void initialize()
{
    START_INITIALIZE;

    /*** HELP
	 NEW_ENTRY("symbolic_name",name_of_the_C_function,FLAG,"symbolic_name(args,..): short description");
	 symbolic_name -> is the name by which the user will access a fitting function
	 name_of_the_C_function -> is the name of the C function
	 FLAG -> is either LINPAR if the are only linear parameteres or NOLINPAR if there are
	 symbolic_name(args,...) -> is the name of the function which will be seen by xfitenv menu
    ***/

    NEW_ENTRY("x0",_x0,LINPAR,"x0: identity function");
    NEW_ENTRY("x1",_x1,LINPAR,"x1: linear function");
    NEW_ENTRY("x2",_x2,LINPAR,"x2: power 2");
    NEW_ENTRY("x3",_x3,LINPAR,"x3: power 3");
    NEW_ENTRY("x4",_x4,LINPAR,"x4: power 4");
    NEW_ENTRY("x5",_x5,LINPAR,"x5: power 5");
    NEW_ENTRY("x6",_x6,LINPAR,"x6: power 6");
    NEW_ENTRY("xn",_xn,NOLINPAR,"xn(p): where p is the fitting parameter in x^p)");
    NEW_ENTRY("exp",_exp,NOLINPAR,"exp(p): where p is the fitting parameter in exp(p*x)");

    CLOSE_INITIALIZE;
}


