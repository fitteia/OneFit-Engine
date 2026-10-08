MROOT=./
RAKU=/usr/bin
ARCH=x86_64
UNAME_S := $(shell uname -s)
OS := $(if $(filter Darwin,$(UNAME_S)),MacOSX,LINUX)
PERLVERSION=5.36
ROOT=$(MROOT)
BINDIR=$(HOME)/bin
PERLCORE=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE

help:
	echo "run make ARCH=<x86_64|aarch64> ROOT=<path>  install"
	echo "Example: make ARCH=aarch64 ROOT=./ install"	

# perl -pi, not sed -i'': macOS's BSD sed reads -i'' as -i and takes the
# next argument as the backup suffix (and has no \s), so the install path
# was never filled in there.
set: 	
	perl -pi -e 's@%OFE-PATH%@$(MROOT)@ if /constant OFE-PATH\s*=/' $(MROOT)/bin/onefite $(MROOT)/t/*.rakutest $(MROOT)/examples/command-line/*.me
	perl -pi -e 's@x86_64@$(ARCH)@ if /x86_64/' $(MROOT)/etc/OFE/default/makefile
	perl -pi -e 's@5\.36@$(PERLVERSION)@ if /PERLVERSION=5\.36/' $(MROOT)/etc/OFE/default/makefile
	perl -pi -e 's@.*@PERLCORE=$(PERLCORE)@ if /PERLCORE=/' $(MROOT)/etc/OFE/default/makefile
#    sed -i'' -e "/OS=/ s@.*@OS=$(OS)@" $(MROOT)/etc/OFE/default/makefile

install: set
	make -C $(ROOT)/../C  OS=$(OS) ROOT=$(ROOT) PERLCORE=$(PERLCORE) BINDIR=$(BINDIR) install
	make -C $(ROOT)/../C  OS=$(OS) ROOT=$(ROOT) BINDIR=$(BINDIR) clean

clean:
	make -C $(ROOT)/../C ROOT=$(ROOT) clean
