#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include "globals.h"

void user_initialize()
{
    START_INITIALIZE;

    /*** HELP
	 NEW_ENTRY("symbolic_name",name_of_the_C_function,FLAG,"symbolic_name(args,..): short description");
	 symbolic_name -> is the name by which the user will access a fitting function
	 name_of_the_C_function -> is the name of the C function
	 FLAG -> is either LINPAR if the are only linear parameteres or NOLINPAR if there are
	 symbolic_name(args,...) -> is the name of the function which will be seen by xfitenv menu
    ***/

    NEW_ENTRY("harmosci",amortecimento,NOLINPAR,"harmosci(A,lam,omega,phi): it uses A*exp(-lam*x)*sin(omega*x+phi)\n");

    CLOSE_INITIALIZE;
}





