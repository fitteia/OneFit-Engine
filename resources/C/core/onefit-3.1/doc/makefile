# $Id: makefile,v 1.2 1998/12/28 11:04:48 pedros Exp $
# DESTDIR as to be defiend by the user:
# 
# make gfitn
# make DESTDIR="/user/home/directory" install-bin
# or
# 
#DESTDIR=/user/home/directory

ROOT=/usr/local
PREFIX=
DESTDIR=$(ROOT)$(PREFIX)
DOCDIR=$(DESTDIR)/doc

clean:
	rm -f *~

install: 
	rm -f *~
	install -d $(DOCDIR)
	install -m 0644 *.htm $(DOCDIR)





