/******************************************************************************/
/*				FUNCSN.C				      */
/******************************************************************************/
#include        <stdio.h>
#include	<math.h>
#include        "globals.h"
#include <string.h>

void	funcsn(x,afunc,ma,par,t,mode)
int	ma,mode;
double 	x,*afunc,t,*par;
{
  int	i;
/**
printf("x=%g ma=%d par[0]=%g t=%s mode=%d\n",x,ma,par[0],t,mode);
printf("mode=%d\n",mode);
**/

  for(i=1; i<=ma; i++) {
      /*
	printf("funcsn %s\n",Func_names[i-1]);
      */
    if(!strncmp(Func_names[i-1],"Perl",4)){
      afunc[i]=(*function[i-1])(x,par,t,mode,Func_names[i-1]); 
    }
    else { 
	if(Func_args[i-1] != NULL)
	    afunc[i]=(*function[i-1])(x,par,t,mode,Func_args[i-1],Func_nargs[i-1]);
	else
	    afunc[i]=(*function[i-1])(x,par,t,mode,NULL,NULL);
    }
  }
  
}
/******************************************************************************/
/*									      */
/******************************************************************************/
