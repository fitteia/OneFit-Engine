OS=LINUX
CC=gcc
F77= gfortran -O3 -fPIC
ARCH=x86_64
ROOT=/home/fitteia
PERLVERSION=5.36
PERLCORE=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE
PREFIX=
LIBNUMBER=4.0.4
INTERFACE=UserLib

# versao linux
LIB= $(LIBGFORTRAN) -L$(PERLCORE) -L$(DESTDIR)/lib -lperl -luserlib -lonefit-$(LIBNUMBER) -lonefit-modelos-$(LIBNUMBER) -lonefit-util-$(LIBNUMBER)  $(MINUIT) -lm 

FLAGS= -D__GNU_LIBRARY__ -D$(OS) -fPIC -O3 -Wall -Werror=implicit-function-declaration -Dbool=char -DHAS_BOOL -I$(PERLCORE) -I$(INCLUDEDIR)

CCFLAGS= -D$(OS) -D_REENTRANT -D_GNU_SOURCE -DDEBIAN -fstack-protector -fno-strict-aliasing -pipe -I$(ROOT)$(PREFIX)/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -fPIC -I$(PERLCORE) -I$(ROOT)$(PREFIX)/include -I$(ROOT)/C/core/onefit-3.1 -I$(ROOT)/C/core/onefit-3.1/modelos -I./

LDFLAGS= -L$(PERLCORE) -shared -L/usr/local/lib -fstack-protector -O2 -g 

LIBS= -lperl -L$(ROOT)$(PREFIX)/lib/ -lonefit-modelos-$(LIBNUMBER) -lonefit-util-$(LIBNUMBER) -lgfortran -lm -I$(ROOT)$(PREFIX)/include

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCPATH := $(shell brew --prefix gcc)
	LIBGFORTRAN = -L$(GCCPATH)/lib/gcc/current
	CC = gcc -std=c99  $(FLAGS)
else
	LIBGFORTRAN=
	CC = gcc $(FLAGS)
endif

lib: *.c *.f
	$(CC) $(CCFLAGS) -c *.c 
	$(F77) -c *.f
	ar -rvs libuserlib.a *.o
	rm *.o

module: *.c *.f $(INTERFACE).i
	swig -perl $(INTERFACE).i
	$(CC) -c $(CCFLAGS) *.c 
	$(F77) -c *.f
	$(CC)  $(LDFLAGS)  *.o -o $(INTERFACE).so $(LIBS)
	rm $(INTERFACE)_wrap.o $(INTERFACE)_wrap.c

module-test:
	perl -e 'use lib "./"; use UserLib; print UserLib::BPP(1,1,1)."\n";'
	perl -e 'use lib "./"; use UserLib; print UserLib::EllipticF(1.5,0.5,10.0)."\n";'
	perl -e 'use lib "./"; use UserLib; print UserLib::iT1inISpara(1e6,300,1e-12,1e-12,1e-10,19)."\n";'

install: lib module
	install -d $(ROOT)$(PREFIX)/lib $(ROOT)$(PREFIX)/include
	install -m 0644 *.dat *.a *.so *.pm *.h $(ROOT)$(PREFIX)/lib
	install -m 0644 *.h $(ROOT)$(PREFIX)/include

clean:
	rm -f *.o *~
