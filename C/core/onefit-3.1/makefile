# $Id: makefile,v 1.12 1999/10/28 15:45:13 pedros Exp $
ARCH=x86_64
OS=LINUX
CC=gcc
ROOT=/home/lfx/fitteia
PREFIX=
DESTDIR=$(ROOT)$(PREFIX)
HOME= $(DESTDIR)
LIBDIR=$(DESTDIR)/lib
BINDIR=$(DESTDIR)/bin
INCLUDEDIR=$(DESTDIR)/include
SRCDIR=$(DESTDIR)/src
#
# the lines below depend on your system
#
PERLVERSION=5.36
PERLLIB=/usr/lib/$(ARCH)-linux-gnu/libperl.so.$(PERLVERSION)
PERLCORE=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE

# run to get above directory: perl -MConfig -e 'print $Config{archlib}'

CERNLIBPATH=/usr/lib
CERNMATH=-lmathlib
CERNPACK=-lpacklib

# to get the above CERNLIBPATH run: locate libmathlib.a

LIBNUMBER=4.0.4
LIBNAME=libonefit-$(LIBNUMBER).a
LIBUTILNAME=libonefit-util-$(LIBNUMBER).a

# versao linux
LIB=  -L$(DESTDIR)/lib -lonefit-$(LIBNUMBER) -lonefit-util-$(LIBNUMBER) -lonefit-modelos-$(LIBNUMBER)   -L$(CERNLIBPATH) $(CERNMATH) $(CERNPACK) -lf2c -lm 
SRC= functions.c tabfunc.c
OBJ= functions.o tabfunc.o

FLAGS= -D$(OS) -O2 -fPIC -Dbool=char -DHAS_BOOL -I$(PERLCORE) -I./ -Wall

#CC = $(CC) $(FLAGS) 

TAR = onefit-$(LIBNUMBER).tar


OBJ= 	gfitn.o funcoes.o readdat.o \
	loadpar.o fsn.o	gchi2_vn.o funcsn.o \
	lfit1n.o usage.o gfitoutp.o \
	ppcovar.o pp3d.o gchi2out.o gfit_out.o plot.o mygnus.o xmgr.o

SRC= 	gfitn.c fitutil.c funcoes.c  readdat.c \
	loadpar.c fsn.c	gchi2_vn.c gfitoutp.c gfit_out.c funcsn.c \
	lfit1n.c usage.c ppcovar.c ludcmp.c \
	lubksb.c gamma.c ppcovar.c pp3d.c gchi2out.c tabfunc.c plot.c mygnus.c getopt.c\
	getopt1.c getopt.h fitenv.c integra.c makefile gfitn.h globals.h \
	struct.h mixed.h actpar.c xmgr.c

UTIL= 	fitutil.o ludcmp.o lubksb.o gamma.o integra.o getopt.o getopt1.o ode2.o

all: install-src Lib fitenv actpar cop fft afactors-xyz ccat

Lib: $(OBJ) $(UTIL)
	$(AR) -rvs  $(LIBNAME) $(OBJ)
	$(AR) -rvs $(LIBUTILNAME) $(UTIL)
	make -C modelos

modelos: 
	make -C modelos

gfitn: 
#	make -C examples 
#	gfitn -l

tar: $(CS1) $(CS2) $(OTHERS)
	tar cvf $(TAR) $(SRC) 
	gzip -9c $(TAR) > $(TAR).gz
	rm $(TAR)

gfitn.o: gfitn.c gfitn.h
	$(CC) $(FLAGS) -c gfitn.c

fitutil.o: fitutil.c
	$(CC)  $(FLAGS) -c fitutil.c

funcoes.o: funcoes.c
	$(CC)  $(FLAGS) -c funcoes.c


readdat.o: globals.h readdat.c
	$(CC)  $(FLAGS) -c readdat.c

fsn.o: fsn.c globals.h
	$(CC)  $(FLAGS) -c fsn.c

gfit_out.o: gfit_out.c globals.h
	$(CC)  $(FLAGS) -c gfit_out.c

gfitoutp.o: gfitoutp.c globals.h
	$(CC)  $(FLAGS) -c gfitoutp.c

funcsn.o: funcsn.c globals.h
	$(CC)  $(FLAGS) -c funcsn.c

loadpar.o: globals.h loadpar.c
	$(CC)  $(FLAGS) -c loadpar.c

gchi2_vn.o: globals.h gchi2_vn.c
	$(CC)  $(FLAGS) -c gchi2_vn.c

lfit1n.o: lfit1n.c
	$(CC)  $(FLAGS) -c lfit1n.c

usage.o: usage.c
	$(CC)  $(FLAGS) -c usage.c

ppcovar.o: ppcovar.c globals.h
	$(CC)  $(FLAGS) -c ppcovar.c

ludcmp.o: ludcmp.c
	$(CC)  $(FLAGS) -c ludcmp.c

lubksb.o: lubksb.c
	$(CC)  $(FLAGS) -c lubksb.c

gamma.o: gamma.c
	$(CC)  $(FLAGS) -c gamma.c

gchi2out.o: gchi2out.c globals.h
	$(CC)  $(FLAGS) -c gchi2out.c

pp3d.o: pp3d.c globals.h
	$(CC)  $(FLAGS) -c pp3d.c

tabfunc.o: tabfunc.c globals.h
	$(CC)  $(FLAGS) -c tabfunc.c

plot.o: plot.c globals.h
	$(CC)  $(FLAGS) -c plot.c

mygnus.o: mygnus.c globals.h
	$(CC)  $(FLAGS) -c mygnus.c

xmgr.o: xmgr.c globals.h
	$(CC)  $(FLAGS) -c xmgr.c -DDEBIAN9

fitenv.o: fitenv.c globals.h
	$(CC)  $(FLAGS) -c fitenv.c

getopt.o: getopt.c
	$(CC)  $(FLAGS) -c getopt.c 

getopt1.o: getopt1.c
	$(CC)  $(FLAGS) -c getopt1.c 

integra.o: integra.c struct.h
	$(CC)  $(FLAGS) -c integra.c

ode2.o: ode2.c struct.h
	$(CC)  $(FLAGS) -c ode2.c

teste: teste.o fitutil.o
	$(CC)  $(FLAGS) -o $(HOME)/bin/teste teste.o $(LIB)

teste.o: teste.c
	$(CC)  $(FLAGS) -c teste.c

clean:
	rm -f *.o *.a  *~ fitenv actpar cop ccat
#	make -C examples clean
	make -C modelos clean
	make -C doc clean
#	make -C gnuplot-3.7.2 clean
#	make -C grace-5.1.14 clean

RMEXE:
	rm -f gfitn

fft: fft.c fft-utils.c
	$(CC)  $(FLAGS) fft.c fft-utils.c -lm fitutil.o -o fft

gfilt: gfilt.c 
	$(CC)  $(FLAGS) gfilt.c -lm fitutil.o -o gfilt

ccat: ccat.c
	$(CC)  $(FLAGS) ccat.c fitutil.o -lm -o ccat

cop: cop.c
	$(CC)  $(FLAGS) fitutil.o cop.c -lm -o cop

fitenv: fitenv.o fitutil.o
	$(CC)  $(FLAGS) -o fitenv fitenv.o fitutil.o -lm

actpar: actpar.c
	$(CC)  $(FLAGS) actpar.c $(LIBUTILNAME) -lm -o actpar 

afactors-xyz: afactors-xyz.c
	$(CC)  $(FLAGS) afactors-xyz.c -lm -O3 -o afactors-xyz

install-fitteia: Lib fft cop gfilt ccat afactors-xyz
	install -d $(BINDIR) $(LIBDIR) $(INCLUDEDIR) 
	install -m 0644 $(LIBNAME) $(LIBUTILNAME) $(LIBDIR)
	install -m 0644 globals.h struct.h mixed.h integra.h $(INCLUDEDIR)
	install -m 0755 fft cop gfilt ccat afactors-xyz $(BINDIR)
	make -C modelos install-lib
	make -C perl install-pcop

install: all
	install -d $(BINDIR) $(LIBDIR) $(LIBEXECDIR) $(INCLUDEDIR) $(PERLDIR)
	install -m 0644 $(LIBUTILNAME) $(LIBNAME) $(LIBDIR)
	install -m 0644 globals.h struct.h mixed.h integra.h $(INCLUDEDIR)
	install cop fitenv actpar afactors-xyz ccat fft gfilt $(BINDIR)
#	make -C examples install
	make -C modelos install
	make -C doc install
	make -C perl install
#	make -C cern install
	make -C $(DIST) install
#	make -C gnuplot-3.7.2 install
#	make -C grace-5.1.14  install
#	make -C Tk-804.027-knoppix3.3 install


install-src:
	make clean
	install -d $(SRCDIR)
	make -C modelos install-src

binary:	install
	cd /tmp; tar zcvf ./onefit-2.0r4-$(DIST).tar.gz bin/ OneFit/; cd -; cp /tmp/onefit-2.0r4-$(DIST).tar.gz ../
