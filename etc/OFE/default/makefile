# OneFit ROOT as to be defiend by the user:
# 
# make ROOT="/path/to/onefit_root_dir" 
# 
#ROOT=/user/home/OneFit

OS=LINUX
ARCH=x86_64
ROOT=/home/fitteia
PREFIX=
DESTDIR=$(ROOT)$(PREFIX)
LIBDIR=$(DESTDIR)/lib
LIBNUMBER=4.0.4
INCLUDEDIR=$(DESTDIR)/include
BINDIR=$(DESTDIR)/bin
EXAMPLESDIR=$(DESTDIR)/examples
NAME=onefit-user
#
# the lines below depend on your system
PERLVERSION=5.36
PERLLIB=/usr/lib/$(ARCH)-linux-gnu/libperl.so.$(PERLVERSION)
PERLROOT=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE
PERLCORE=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE
# run to get above directory: perl -MConfig -e 'print $Config{archlib}'

CERNLIBPATH=/usr/lib
CERNMATH=-lmathlib
CERNPACK=-lpacklib
F2C=

CERNLIB= -L$(CERNLIBPATH)  $(CERNMATH) $(CERNPACK) $(F2C)
#MINUIT=$(CERNLIB)
MINUIT= -lminuit -lgfortran

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCPATH := $(shell brew --prefix gcc)
	LIBGFORTRAN = -L$(GCCPATH)/lib/current
else
	LIBGFORTRAN=
endif

# versao linux
LIB= $(LIBGFORTRAN) -L$(PERLCORE) -L$(DESTDIR)/lib -lperl -luserlib -lonefit-$(LIBNUMBER) -lonefit-modelos-$(LIBNUMBER) -lonefit-util-$(LIBNUMBER)  $(MINUIT) -lm 

FLAGS=  -D$(OS) -fPIC -O3 -Wall -Werror=implicit-function-declaration -Dbool=char -DHAS_BOOL -I$(PERLCORE) -I$(DESTDIR)/include

CCFLAGS=-D_REENTRANT -D_GNU_SOURCE -DDEBIAN -fstack-protector -fno-strict-aliasing -pipe -I/usr/local/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O3 -g -fPIC -I$(PERLCORE)  -I$(ONEFIT)/include

LDFLAGS= -L$(PERLCORE) -lperl -shared -L/usr/local/lib -fstack-protector -O3 -g

CC = gcc $(FLAGS)

#CC=gcc -I/usr/lib/perl/5.6.1/CORE -I/include

objects := $(patsubst %.c,%.o,$(wildcard *.c))

gfitn : clean module $(objects)
	$(CC) $(objects) $(LIB) -o $(NAME) 

module: AuxCode.c AuxCode.i
	swig -perl AuxCode.i
	gcc -c $(CCFLAGS) AuxCode.c AuxCode_wrap.c -I$(DESTDIR)/include
	gcc $(LDFLAGS)  AuxCode.o AuxCode_wrap.o -o AuxCode.so
	rm AuxCode_wrap.o AuxCode_wrap.c
#	mv AuxCode.pm $ROOT/public_html/$numero/
#	mv AuxCode.so $ROOT/public_html/$numero/

clean:
	rm -f *.o *~ *_ *.gph *.gp_ gnu*

install: 
	rm -f *.o *~ *_ *.gph *.gp_ gnu*
	install -d $(EXAMPLESDIR)
	install --mode=0644 [1-9,a-z]* $(EXAMPLESDIR)
