ROOT=/home/lfx/fitteia
PREFIX=
OS=LINUX
PERLCORE=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE

install:
	make OS=$(OS) ROOT=$(ROOT) PERLCORE=$(PERLCORE) -C core/onefit-3.1 install-fitteia
	make OS=$(OS) ROOT=$(ROOT) PERLCORE=$(PERLCORE) -C core/onefit-3.1/perl install-pcop
	make OS=$(OS) ROOT=$(ROOT) PERLCORE=$(PERLCORE) -C core/onefit-3.1/doc install
	make OS=$(OS) ROOT=$(ROOT) PERLCORE=$(PERLCORE) -C local install

clean:
	rm -f *~
	make ROOT=$(ROOT) -C core/onefit-3.1 clean
	make ROOT=$(ROOT) -C local clean
