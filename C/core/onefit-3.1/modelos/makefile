# $Id: makefile,v 1.5 1998/07/04 10:54:48 pedros Exp $
ARCH=x86_64
ROOT=/home/lfx/fitteia
PREFIX=
DESTDIR=$(ROOT)/$(PREFIX)
LIBDIR=$(DESTDIR)/lib
LIBNUMBER=4.0.4
FLAGS= -DLINUX -O2 -fPIC -I$(ROOT)/C/core/onefit-3.1/
CC= gcc $(FLAGS)
F77= gfortran -fPIC
SRCDIR=$(DESTDIR)/src

LIBNAME=libonefit-modelos-$(LIBNUMBER).a


OBJ=    dif_phase.o dimer.o fij.o lor.o odf.o rot.o sdvilfsa7.o \
        sdvilf5.o sdvilfjwn11_2.o sdvilfn11_1.o sdvilfn11_2.o sdvilfn11_201.o \
        sdvilfsa11.o sdvilfsa5.o sdvilfsa5Dll0.o sdvilfsa11_0.o sdvilfsa13_0.o\
        t1rodelta.o \
        t1torrey.o t1_rot_vold.o t1vold_new.o t1vold_abc.o Ftotal.o

SRC=	 dif_phase.c dimer.c fij.c lor.c odf.c rot.c sdvilfsa7.c \
        sdvilf5.c sdvilfjwn11_2.c sdvilfn11_1.c sdvilfn11_2.c sdvilfn11_201.c\
        sdvilfsa11.c sdvilfsa5.c sdvilfsa5Dll0.c sdvilfsa11_0.c sdvilfsa13_0.c\
        t1rodelta.c Ftotal.c \
        t1torrey.c t1_rot_vold.c t1vold_new.f t1vold_abc.f makefile struct.h \
        struf.h plot.h var.h

Lib: $(OBJ)
	$(AR) -rvs $(LIBNAME) $(OBJ)

install-lib: Lib
	install -d $(LIBDIR)
	install -m 0644 $(LIBNAME) $(LIBDIR)

tar: $(SRC)
	tar zcvf modelos.tar $(SRC) 

dif_phase.o: dif_phase.c
	$(CC) -c dif_phase.c

dimer.o: dimer.c
	$(CC) -c dimer.c

fij.o: fij.c
	$(CC) -c fij.c

lor.o: lor.c
	$(CC) -c lor.c

odf.o: odf.c
	$(CC) -c odf.c

odf_util.o: odf_util.c
	$(CC) -c odf_util.c

rot.o: rot.c
	$(CC) -c rot.c

sdvilfsa7.o: sdvilfsa7.c
	$(CC) -c sdvilfsa7.c

sdvilf5.o: sdvilf5.c
	$(CC) -c sdvilf5.c

sdvilfsa5Dll0.o: sdvilfsa5Dll0.c
	$(CC) -c sdvilfsa5Dll0.c

sdvilfjwn11_2.o: sdvilfjwn11_2.c
	$(CC) -c sdvilfjwn11_2.c

sdvilfn11_1.o: sdvilfn11_1.c
	$(CC) -c sdvilfn11_1.c

sdvilfn11_2.o: sdvilfn11_2.c
	$(CC) -c sdvilfn11_2.c

sdvilfn11_201.o: sdvilfn11_201.c
	$(CC) -c sdvilfn11_201.c

sdvilfsa11_0.o: sdvilfsa11_0.c
	$(CC) -c sdvilfsa11_0.c

sdvilfsa11.o: sdvilfsa11.c
	$(CC) -c sdvilfsa11.c

sdvilfsa13_0.o: sdvilfsa13_0.c
	$(CC) -c sdvilfsa13_0.c

sdvilfsa5.o: sdvilfsa5.c
	$(CC) -c sdvilfsa5.c

t1rodelta.o: t1rodelta.c
	$(CC) -c t1rodelta.c

t1torrey.o: t1torrey.c
	$(CC) -c t1torrey.c -I../

t1_rot_vold.o: t1_rot_vold.c
	$(CC) -c t1_rot_vold.c

t1vold_new.o: t1vold_new.f
	$(F77) -c t1vold_new.f

t1vold_abc.o: t1vold_abc.f
	$(F77) -c t1vold_abc.f

Ftotal.o: Ftotal.c
	$(CC) -I$(DESTDIR)/include -I../ -I./ -c Ftotal.c

clean:
	rm -f *.o *.a *~

install-src:
	install -d $(SRCDIR)
	install -m 0644 *.c $(SRCDIR)

