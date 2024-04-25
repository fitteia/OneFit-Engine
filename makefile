ROOT=./
RAKU=/usr/bin
ARCH=x86_64
OS=LINUX
PERLVERSION=5.36
BINDIR=$(ROOT)/bin

help:
	echo "run make ARCH=<x86_64|aarch64> ROOT=<path>  install"
	echo "Example: make ARCH=aarch64 ROOT=./ install"	

set: 	
	sed -i'' -e "/constant OFE-PATH\s*=/ s@%OFE-PATH%@$(ROOT)@" $(ROOT)/bin/onefite
	sed -i'' -e "/constant OFE-PATH\s*=/ s@%OFE-PATH%@$(ROOT)@" $(ROOT)/t/*.rakutest
	sed -i'' -e "/x86_64/ s@x86_64@$(ARCH)@" $(ROOT)/etc/OFE/default/makefile
	sed -i'' -e "/PERLVERSION=5.36/ s@5.36@$(PERLVERSION)@" $(ROOT)/etc/OFE/default/makefile

install: set
	make -C $(ROOT)/C install
	make -C $(ROOT)/C clean

clean:
	make -C $(RROT)/C clean
