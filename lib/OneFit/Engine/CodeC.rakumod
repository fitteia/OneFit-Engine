unit module OneFit::Engine::CodeC;

class template is export {
    has $.head = q:to/EOT/;
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "struct.h"
#include "userlib.h"
#include "ndata.h"
#define pi 3.1415926
#define PI 3.1415926
#define pI 3.1415926
#define Pi 3.1415926
#define SIGN(a) ((a) > 0.0 ? 1 : -1)
EOT


    has $.struct = qq:to/EOA/;
typedef struct    \{
  int                n;
  double             t;
  double             x0;
  double             x;
  double             h;
  double             y0[NODE1];
  double             y[NODE1];
  double             (*dy[NODE1])();
  Parameter          par[50];
\} ODEn;

static int ruku4thnode1_first_time=1;

EOA

    has $.RungeKutta = qq:to/EOB/;
double RuKu4thNODE1(double xxx, ODEn *oden)
\{
  double h,n,af;
  int i;

  n=oden->n;
  h=oden->h;
  if(ruku4thnode1_first_time==1)\{
    _x_[0]=_x_[1]=oden->x0;
    for(i=0;i<n;i++)\{ _y_[i][0]=_y_[i][1]=oden->y0[i]; \}
    ruku4thnode1_first_time=0;
  \}
  if(xxx<=_x_[0])\{
    _x_[0]=_x_[1]=oden->x0;
    for(i=0;i<n;i++)\{ _y_[i][0]=_y_[i][1]=oden->y0[i]; \}
  \}
  do \{
    _x_[0]=_x_[1];
    for(i=0;i<n;i++)\{ _y_[i][0]=_y_[i][1]; \}

    oden->x=_x_[0];
    for(i=0;i<n;i++)\{ oden->y[i]=_y_[i][0]; \}
    for(i=0;i<n;i++)\{ _df_[i][0]=h*oden->dy[i](oden); \}

    oden->x=_x_[0]+0.5*h;
    for(i=0;i<n;i++)\{ oden->y[i]=_y_[i][0]+0.5*_df_[i][0]; \}
    for(i=0;i<n;i++)\{ _df_[i][1]=h*oden->dy[i](oden); \}

    oden->x=_x_[0]+0.5*h;
    for(i=0;i<n;i++)\{ oden->y[i]=_y_[i][0]+0.5*_df_[i][1]; \}
    for(i=0;i<n;i++)\{ _df_[i][2]=h*oden->dy[i](oden); \}

    oden->x=_x_[0]+h;
    for(i=0;i<n;i++)\{ oden->y[i]=_y_[i][0]+_df_[i][2]; \}
    for(i=0;i<n;i++)\{ _df_[i][3]=h*oden->dy[i](oden); \}

    _x_[1]=_x_[0]+h;
    for(i=0;i<n;i++)\{ _y_[i][1]=_y_[i][0]+(_df_[i][0]+2*_df_[i][1]+2*_df_[i][2]+_df_[i][3])/6.0; \}
   \} while (xxx>=_x_[1]);
\n\n
   af= (_y_[SOLVE-TO][1]-_y_[SOLVE-TO][0])/(_x_[1]-_x_[0])*(xxx-_x_[0])+_y_[SOLVE-TO][0];
   return af;
\}
EOB

}


class Code is export {
    has %!engine;
    has @!blocks;
    has $!path = '.';
	
    method path ($folder) {$!path = $folder; self }
    
    method cwrite (%e,@blocks,@Functions,:$path) {
	$!path =$path if $path.defined;
	%!engine=%e;
	@!blocks=@blocks;
	
	my $auxcode = (%!engine<AuxCode>.defined and %!engine<AuxCode>.Bool)
			  ?? %!engine<AuxCode>
			  !! q:to/EOT/;
#include <math.h>

#define pi 3.1415926

double template1(double x, double a)
{
  double result;
  return a;
}

double template2(double x, double a)
{
  double result;
  result=a*x;
  return result;
}
EOT
	my $auxdeclar = (%!engine<AuxDeclar>.defined and %!engine<AuxDeclar>.Bool) ?? %!engine<AuxDeclar> !! "double template1(), template2()";
	
	dir($!path, :test(/\.c/)).race.map({ $_.unlink if $_.IO.f });

	for (0 ..^ @Functions.elems).race { @Functions[$_].write-c(path => $!path, name => "userfitf$_" ,x => %!engine<X>.Str ,y => %!engine<Y>.Str,t => %!engine<T>.Str, AuxDeclar => ($auxdeclar.defined) ?? $auxdeclar.subst(/double|int|void|string|<ws>/,'') !! '') }

	
	if ($auxcode.defined) {
	    $auxcode = "#include <stdio.h>\n" ~ $auxcode unless $auxcode.contains("stdio.h");
	    $auxcode = "#include <math.h>\n" ~ $auxcode unless $auxcode.contains("math.h");
	    $auxcode = "#include \"ndata.h\"\n" ~ $auxcode unless $auxcode.contains("ndata.h");
	    $auxcode = "#include \"mixed.h\"\n" ~ $auxcode unless $auxcode.contains("mixed.h");
	    "$!path/AuxCode.c".IO.spurt: $auxcode;
	}
	
	if ($auxdeclar.defined) {
	    $auxdeclar = $auxdeclar.split(';').head;
	    $auxdeclar ~~ s:g/double|int|void|string|\(|\)//;
	    my @a;
	    for $auxdeclar.split(/\s*','\s*/) { @a.push: self!get-auxfunc($_,$auxcode) }
	    "$!path/AuxCode.i".IO.spurt: "%module AuxCode\n\n%inline " ~
								       "%\{\n" ~
								       @a.join(";\n") ~
								       ";\n%\}\n" ~
								       @a.join(";\n") ~
								       ";\n";
	}
	else { "$!path/AuxCode.i".IO(:e).unlink }
	self!write-tabfunc(@Functions>>.parameters);
	self!write-ndata-h();
	self	
    }
    
    method !get-auxfunc ($func,$code) {
	my $result="";
	for ($code.split('{')) {
	    if (/$func'('/ and $func.Bool) {
		$result = $_.split(/$func'('/)[1].split(')')[0];
		$result = $result.subst(/\s+|'double'|'('|')'\n*/,'',:g).subst(',',', double ',:g);
	    }
	}
	return "extern double " ~ $func.subst(/^\s*/,'') ~ { ($result) ?? "(double $result)" !! "()" }();
    }
    
    method !write-ndata-h () {
	my $t = %!engine<T>;
	my @tpars=@!blocks.head.T.words;
	
	my $TXT="";
	for (0 .. @tpars.elems) {
	    $TXT ~= "#define $t" ~ "_$_ (Matrix_$t\[(int) ($t)\]\[$_\])\n";
	}
	for (0 ..^ @!blocks.elems) -> $i {
	    FIRST {
		$TXT ~= "\nstatic double Matrix_$t\[\]\[{ @tpars.elems + 1 }\]=\{\n";
		$TXT ~= "\{" ~ "{ { (0 .. $^a) >>*>> 0 }(@tpars).join(',') }" ~ "\},\n";
	    }
	    my @a = ($i+1);
	    $TXT ~= "\{" ~ "{ @a.push(@!blocks[$i].T.words.Slip).join(',') }" ~ "\},\n";
	    LAST { $TXT ~= "\};\n"; }
	}
	"$!path/ndata.h".IO.spurt: $TXT;
	self
    }

    method !write-tabfunc (@parameters) {
	my $txt = q:to/EOT/;
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#ifndef MacOSX
   #include <malloc.h>
#endif
#include <stdlib.h>
#include "globals.h"
#include "struct.h"
#include "userlib.h"

void user_initialize()
{
    START_INITIALIZE;

EOT
        for (0 ..^ @parameters) -> $i {
	    $txt = $txt ~ "        NEW_ENTRY(\"userfitf{$i}\",userfitf{$i},NOLINPAR,\"userfitf{$i}({@parameters[$i].join(',')}): \");\n";
        }    
	$txt = $txt ~ q:to/END/;

    CLOSE_INITIALIZE;
}
END
        "$!path/tabfunc.c".IO.spurt: $txt;
        self
    }
    
    method compile (Bool :$quiet) {
#	my $path = ::('OFE-PATH').subst("rakumod/OFE","");
	my $path = ::('OFE-PATH');
	"$!path/compile.log".IO(:e).unlink;
	my $fho = open :a, "$!path/compile.log";
	my $proc = shell "cd $!path; make ROOT=$path -f $path/etc/OFE/default/makefile gfitn", :out($fho), :err($fho);
	$fho.close;
	if "$!path/compile.log".IO.lines.Array.pop.contains("onefit-user") {
	    say "C code extracted compiled OK!" unless $quiet
	}
	else {
	    say "C code extracted compiled with errors. Please check the compile.log file for more information."
	}
	
	$proc = shell "cd $!path; make ROOT=$path -f $path/etc/OFE/default/makefile clean", :out, :err;
	self
    }

}
