#include <math.h>
#include "Umath.h"

double factorial(double x)
{
  int i;
  i = (int) x;
  if(i==0) { return 1.0; }
  else {return i*factorial(i-1); }
}
