#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

void usage()
{
  printf("\n(The idea used in the first version of this program");
  printf("\n was the result of a suggestion of Carlos Cruz");
  printf("\n Thank you Carlos for that challenge)\n\n");

  printf("usage: onefit [-@options_file] [-short_options] [--long_options=] datafiles [-short_options] [--long_options=] datafiles ...\n");
  printf("short options: -DFGOPgdo@ntpzleh\n");
  printf("long  options: autox_scale, autox, autoy_scale, autoy, fit, nofit,plot\n");
  printf("               noplot, label, nolabel, errors, device, Outdev, xlabel\n");
  printf("               ylabel, xscale, yscale, point_type, ptype, gph, num_gph_points\n");
  printf("               num, Function, Parameter, Par, xm, xM, ym, yM, Gnuplot\n");
  printf("               Gnu, xy, xyz, xyt, xmgr, grbatch, xmgr_par_file, PERL_file,display\n");

  printf("\n");
  printf("--grbatch=image_format_type (eps,jpg,png,tif,ps,pdf,pnm,pgm,pcx,wmf\n");
  printf("-D, --display (default yes)\n");
  printf("-F, --Function=<function>\n");
  printf("-P, --Parameter=, --Par=<parfile>\n");
  printf("-G, --Gnuplot=, --Gnu=<gnuplot file> (default: gfitn.gnu)\n");
  printf("--xmgrace <use xmgrace for plotting> (default: no)\n");
  printf("-d, --device=<screen or printer type> (default: gnuplot default)\n");
  printf("-O, --Outdev=<graphic output file> (default: none)\n");
  printf("-o <output file> (default: gfitn.out)\n");
  printf("-g <to plot graphics on device> (default: yes)\n");
  printf("-p, --plot <to generate graphics files> (default: yes)\n");
  printf("-f, --fit <to fit> (default: yes)\n");
  printf("-n[fpgD] <no fit or plot or graphic or display>\n");
  printf("-t <xyt type of data files> (default: xy)\n");
  printf("-z <xyz type of data files> (not implemented)\n");
  printf("-e, --errors (not implemented)\n");
  printf("-l <list compliled fitting function names>\n");
  printf("-h <help>\n");

  printf("\nTry x1fit (xonefit) as a GUI fitting environment!\n");
  exit(0);
}
