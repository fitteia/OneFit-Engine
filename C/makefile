ROOT=/home/lfx/fitteia
PREFIX=
OS=Linux

install:
	make ROOT=$(ROOT) -C core/onefit-3.1 install-fitteia
	make ROOT=$(ROOT) -C core/onefit-3.1/perl install-pcop
	make ROOT=$(ROOT) -C core/onefit-3.1/doc install
	make ROOT=$(ROOT) -C local install

clean:
	rm -f *~
	make ROOT=$(ROOT) -C core/onefit-3.1 clean
	make ROOT=$(ROOT) -C local clean
