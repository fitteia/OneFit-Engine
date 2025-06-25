/*****************************************************************************/
/*            GFITN.C                     */
/*****************************************************************************/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "fsn.h"
#include "gfit_out.h"
#include "gchi2_vn.h"
#include "gfitoutp.h"

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void fcn(int *npar, double g[], double *f, double par[], int *iflag)
// int   *npar,*iflag;
// double   g[],*f,par[];
{
   double  *chis;

   switch(*iflag) {
   case 1:
     break;

   case 3:
     gfit_outpg(par);
     gfit_outgraf(par);  /* to generate the graphic files */
     break;

   case 4:
     chis = dvector(1,NT);
     *f = gchi2_vn(par,chis);

     free_dvector(chis,1,NT);
     break;
     
   default:
     break;
   }
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

