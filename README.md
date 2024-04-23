# onefite
The One Fit Engine (OneFite) is fitteia's internal fitting core, made available from the command line through an SSH connection. OneFite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process


To clone: git clone git@github.com:fitteia/onefite.git

To install in MacOS linux: make ROOT=$PWD RAKU=$(type raku | awk '{print $3}') install

To install in Mac M2 iOS: make OS=MacOSX ROOT=$PWD RAKU=$(type raku | awk '{print $3}') install

to install in Linux: make ROOT=$PWD RAKU=$(type raku | awk '{print $4}' | sed -e 's#[\\(\\)]##g') install


Fresh instalation
For Debian "bookworm"

apt install libper-dev \
    	    swig \
	    gcc \
	    dpkg-dev \
	    libgfortran5 \
	    cfortran \
	    gfortran \
	    grace \
	    imagemagick \
	    texlive-font-utils \
	    ghostscript \
	    libcgi-pm-perl \
	    zip \
	    pdftk \
	    sudo 
	    
curl -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/cernlib-base-dev_20061220+dfsg3-4.4_all.deb -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/cernlib-base_20061220+dfsg3-4.4_all.deb -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libkernlib1-dev_20061220+dfsg3-4.4_amd64.deb -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libkernlib1-gfortran_20061220+dfsg3-4.4_amd64.deb -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libmathlib2-dev_20061220+dfsg3-4.4_amd64.deb -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libmathlib2-gfortran_20061220+dfsg3-4.4_amd64.deb -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libpacklib1-gfortran_20061220+dfsg3-4.4_amd64.deb -O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libpacklib1-dev_20061220+dfsg3-4.4_amd64.deb

dpkg -i cernlib-base_20061220+dfsg3-4.4_all.deb \
dpkg -i cernlib-base-dev_20061220+dfsg3-4.4_all.deb \
dpkg -i libkernlib1-gfortran_20061220+dfsg3-4.4_amd64.deb \
dpkg -i libkernlib1-dev_20061220+dfsg3-4.4_amd64.deb \
dpkg -i libpacklib1-gfortran_20061220+dfsg3-4.4_amd64.deb \
dpkg -i libmathlib2-gfortran_20061220+dfsg3-4.4_amd64.deb \
apt install --fix-broken \
dpkg -i libmathlib2-gfortran_20061220+dfsg3-4.4_amd64.deb \
dpkg -i libpacklib1-dev_20061220+dfsg3-4.4_amd64.deb \
dpkg -i libmathlib2-dev_20061220+dfsg3-4.4_amd64.deb \




vi /etc/ImageMagick-6/policy.xml to enable ghostscript formats

zef install Inline::Perl5


mkdir ~/bin 
mkdir ~/Downloads \
git clone git@github.com:fitteia/onefite.git \
or\
git clone https://github/fitteia/onefite.git \
cp install-onefite-from-git ../bin/ \
cd ~/bin \
ln -s ../onefite/bin/afactors-xyz \
ln -s ../onefite/bin/cop \
ln -s ../onefite/bin/pcop \
ln -s ../onefite/bin/gfilt \
ln -s ../onefite/bin/fft \
ln -s ../onefite/bin/onefite \

cd ~/Dowloads & onefite test
