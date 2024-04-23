# $Id: makefile,v 2.1 2004/01/26 10:04:02 pedros Exp $
# DESTDIR as to be defiend by the user:
# 
# make gfitn
# make DESTDIR="/user/home/directory" install-bin
# or
# 
ROOT=/home/lfx/fitteia
PREFIX=
DESTDIR=$(ROOT)$(PREFIX)
LIBDIR=$(DESTDIR)/lib
INCLUDEDIR=$(DESTDIR)/include
BINDIR=$(DESTDIR)/bin
ROOTBINDIR=$(ROOT)/bin
PERLDIR=$(DESTDIR)/perl
ICONS=$(DESTDIR)/images
clean:
	rm -f *~ *.flc

install-pcop: 
	install -d $(BINDIR)
	install -m 0755 pcop $(BINDIR)
install: 
	rm -f *~ *.flc
	install -d $(PERLDIR)
	install -d $(ROOTBINDIR)
	install -d $(BINDIR)
	install -d $(ICONS)
	install -m 0644 *.gif *.xpm *.bmp $(ICONS)
	install -m 0755 xonefit $(BINDIR)
	install -m 0755 x1fit $(ROOTBINDIR)
	install -m 0644 xonefit.pm $(PERLDIR)
	install -m 0755 text2excel.pl excel2text.pl parfilt outfilt pcop $(BINDIR)
#	cd $(ROOTBINDIR); ln -s $(BINDIR)/xonefit x1fit; ln -s $(BINDIR)/xonefit X1fit; ln -s $(BINDIR)/pcop; ln -s $(BINDIR)/xonefit; 
	cd $(ROOTBINDIR);  ln -s $(BINDIR)/pcop



