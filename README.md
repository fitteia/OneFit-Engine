# OneFit_Engine
The OneFit Engine (OneFit-E) is fitteia's internal fitting core, made available from the command line through an SSH connection. OneFite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process

Fresh instalation
For Debian "bookworm"

apt install libperl-dev swig gcc dpkg-dev libgfortran5 cfortran gfortran grace imagemagick texlive-font-utils ghostscript libcgi-pm-perl zip pdftk sudo raku
	    
curl 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/cernlib-base-dev_20061220+dfsg3-4.4_all.deb 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/cernlib-base_20061220+dfsg3-4.4_all.deb 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libkernlib1-dev_20061220+dfsg3-4.4_amd64.deb 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libkernlib1-gfortran_20061220+dfsg3-4.4_amd64.deb 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libmathlib2-dev_20061220+dfsg3-4.4_amd64.deb 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libmathlib2-gfortran_20061220+dfsg3-4.4_amd64.deb 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libpacklib1-gfortran_20061220+dfsg3-4.4_amd64.deb 
-O http://ftp.de.debian.org/debian/pool/main/c/cernlib/libpacklib1-dev_20061220+dfsg3-4.4_amd64.deb

dpkg -i cernlib-base_20061220+dfsg3-4.4_all.deb 
dpkg -i cernlib-base-dev_20061220+dfsg3-4.4_all.deb 
dpkg -i libkernlib1-gfortran_20061220+dfsg3-4.4_amd64.deb 
dpkg -i libkernlib1-dev_20061220+dfsg3-4.4_amd64.deb 
dpkg -i libpacklib1-gfortran_20061220+dfsg3-4.4_amd64.deb 
dpkg -i libmathlib2-gfortran_20061220+dfsg3-4.4_amd64.deb 
apt install --fix-broken 
dpkg -i libmathlib2-gfortran_20061220+dfsg3-4.4_amd64.deb 
dpkg -i libpacklib1-dev_20061220+dfsg3-4.4_amd64.deb 
dpkg -i libmathlib2-dev_20061220+dfsg3-4.4_amd64.deb 


vi /etc/ImageMagick-6/policy.xml to enable ghostscript formats

zef install Inline::Perl5

mkdir ~/bin ~/Downloads

git clone https://github.com/fitteia/OneFit-Engine.git 

(or git clone git@github.com:fitteia/OneFit-Engine.git )

cd OneFit-Engine && ./INSTALL

