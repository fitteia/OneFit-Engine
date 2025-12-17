OS=LINUX
CC=gcc
F77=gfortran -fPIC -funroll-loops -ffast-math  -fdefault-double-8 -std=legacy --fixed-line-length-none -fd-lines-as-comments -fbackslash
ARCH=x86_64
ROOT=/home/fitteia
PERLVERSION=5.36
PERLCORE=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE
PREFIX=
LIBNUMBER=4.0.4
INTERFACE=UserLib

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCPATH := $(shell brew --prefix gcc)
	LIBGFORTRAN = -L$(GCCPATH)/lib/gcc/current
else
	LIBGFORTRAN=
endif

CCFLAGS= -D$(OS) -O3 -g -fpic -pipe -I$(PERLCORE) -I$(ROOT)$(PREFIX)/include 

#CCFLAGS= -D$(OS) -D_REENTRANT -D_GNU_SOURCE -DDEBIAN -fstack-protector -fno-strict-aliasing -pipe -I$(ROOT)$(PREFIX)/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O3 -g -fPIC -I$(PERLCORE) -I$(ROOT)$(PREFIX)/include -I$(ROOT)/C/core/onefit-3.1 -I$(ROOT)/C/core/onefit-3.1/modelos

LDFLAGS= $(LIBGFORTRAN) -L$(PERLCORE) -shared -L/usr/local/lib -fstack-protector -O3 -g -lgfortran
LIBS= -lperl -L$(ROOT)$(PREFIX)/lib/ -lonefit-modelos-$(LIBNUMBER) -lonefit-util-$(LIBNUMBER) -I$(ROOT)$(PREFIX)/include

CSRCS := $(wildcard *.c)
FSRCS := $(wildcard *.f)

# turn them into object names
OBJS  := $(CSRCS:.c=.o) $(FSRCS:.f=.o)

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

%.o: %.f
	gfortran -O3 -c $< -o $@

lib: $(OBJS) 
#	$(CC) $(CCFLAGS) -c *.c
	ar -rvs libuserlib.a *.o

module: *.c  $(INTERFACE).i
	swig -perl $(INTERFACE).i
	$(CC) -c $(CCFLAGS) *.c 
	$(CC)  $(LDFLAGS)  *.o -o $(INTERFACE).so $(LIBS)
	rm $(INTERFACE)_wrap.o $(INTERFACE)_wrap.c

module-test:
	perl -e 'use lib "./"; use UserLib; print UserLib::BPP(1,1,1)."\n";'
	perl -e 'use lib "./"; use UserLib; print UserLib::EllipticF(1.5,0.5,10.0)."\n";'
	perl -e 'use lib "./"; use UserLib; print UserLib::iT1inISpara(1e6,300,1e-12,1e-12,1e-10,19)."\n";'

install: lib 
	install -d $(ROOT)$(PREFIX)/lib $(ROOT)$(PREFIX)/include
	install -m 0644 *.dat *.a *.h $(ROOT)$(PREFIX)/lib
	install -m 0644 *.h $(ROOT)$(PREFIX)/include

clean:
	rm -f *.o *~
