ROOT=./
RAKU=/usr/bin
ARCH=x86_64
OS=LINUX
PERLVERSION=5.36

help:
	echo "run make ARCH=<x86_64|aarch64> ROOT=<path> RAKU=<path/raku> install"
	echo "Example: make ARCH=aarch64 ROOT=./ RAKU=/usr/bin/raku install"	

set: 	
	sed -i'' -e "/#!/ s@#!.*@#!$(RAKU)@ ; /use lib/ s@.*@use lib \'$(ROOT)/rakumod/OFE\';@ ; /constant OFE-PATH\s*=/ s@.*@constant OFE-PATH = \"$(ROOT)/rakumod/OFE\";@" bin/onefite
	sed -i'' -e "/#!/ s@#!.*@#!$(RAKU)@ ; /use lib/ s@.*@use lib \'$(ROOT)/rakumod/OFE\';@ ; /constant OFE-PATH\s*=/ s@.*@constant OFE-PATH = \"$(ROOT)/rakumod/OFE\";@" t/*.rakutest
	sed -i'' -e "/x86_64/ s@x86_64@$(ARCH)@" etc/OFE/default/makefile
	sed -i'' -e "/PERLVERSION=5.36/ s@5.36@$(PERLVERSION)@" etc/OFE/default/makefile

install: set
	make -C C install
	make -C C clean

clean:
	make -C C clean
