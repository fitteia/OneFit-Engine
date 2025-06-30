MROOT=./
RAKU=/usr/bin
ARCH=x86_64
OS=LINUX
PERLVERSION=5.36
ROOT=$(MROOT)
BINDIR=$(HOME)/bin
PERLCORE=/usr/lib/$(ARCH)-linux-gnu/perl/$(PERLVERSION)/CORE

help:
	echo "run make ARCH=<x86_64|aarch64> ROOT=<path>  install"
	echo "Example: make ARCH=aarch64 ROOT=./ install"	

set: 	
	sed -i'' -e "/constant OFE-PATH\s*=/ s@%OFE-PATH%@$(MROOT)@" $(MROOT)/bin/onefite
	sed -i'' -e "/constant OFE-PATH\s*=/ s@%OFE-PATH%@$(MROOT)@" $(MROOT)/t/*.rakutest
	sed -i'' -e "/constant OFE-PATH\s*=/ s@%OFE-PATH%@$(MROOT)@" $(MROOT)/examples/command-line/*.me
	sed -i'' -e "/x86_64/ s@x86_64@$(ARCH)@" $(MROOT)/etc/OFE/default/makefile
	sed -i'' -e "/PERLVERSION=5.36/ s@5.36@$(PERLVERSION)@" $(MROOT)/etc/OFE/default/makefile
	sed -i'' -e "/PERLCORE=/ s@.*@PERLCORE=$(PERLCORE)@" $(MROOT)/etc/OFE/default/makefile
	sed -i'' -e "/OS=/ s@.*@OS=$(OS)@" $(MROOT)/etc/OFE/default/makefile

install: set
	make -C $(ROOT)/C  OS=$(OS) ROOT=$(ROOT) PERLCORE=$(PERLCORE) BINDIR=$(BINDIR) install
	make -C $(ROOT)/C  OS=$(OS) ROOT=$(ROOT) BINDIR=$(BINDIR) clean

clean:
	make -C $(ROOT)/C ROOT=$(ROOT) clean
