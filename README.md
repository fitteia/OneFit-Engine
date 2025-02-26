# OneFit Engine
The OneFit Engine (onefite) is fitteia's internal fitting core, made available from the command line through an SSH connection. onefite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process. For now the we use the Artistic License 2.0.


## Fresh instalation

OneFit  Engine package is prepared for instalation in Debian

### Windows

Install Oracle VM VirtualBox in your Windows host.\
Download Debian netinst iso file for amd64\
Creat a new guest VM with at least 4 cores, 6 GB HD and 4 GB of RAM\
(ATTENTION: select "Skip the Unattended Guest Installation" to avoid a full debian installation)  
Edit the Network settings and for a NAT network, in Advanced settings -> Port Forwarding add\
port 8122 to localhost and 22 to guest (for ssh connections using puTTY)\
port 8180 to localhost and 80 to guest (for html connections)\
port 8142 to localhost and 8142 for guest (for OneFit-Engine connection using html)\

In the Host: ssh user@localhost -P 8122; http://localhost:8180; http://localhost:8142

Install suggestion: FileZilla (https://filezilla-project.org) and curl (https://curl.se/windows/).
	            When using FileZilla use sftp://localhost:8122/ to use ssh protocol

### Mac M2

Install UTM\
Download Debian for arm64\
Created a new guest Virtualize VM for Linux Architecture ARM64 (aarch64) with at least 8 GB HD, 4 GB of RAM, default number of cores.

In the Host: ssh user@192.168.64.11; http://192.168.64.11; http://192.168.64.11:8142

Install suggestion: curl

### APT based Linux distros (Debian, Ubuntu, Linux Mint,...)

Ex: Debian 12 "bookworm" and Ubuntu 24.04.1 LTS "Noble Numbat"

Suggestion: minimal installation; when at tasksel stage select "SSH server" and "Web server" ("Laptop" might be an option) all other packages will be installed later.\
Create a user account, ex: ofe

#### First login with username ofe

ofe@bookworm:\~$ su - 

root@bookworm\~# apt install git sudo && usermod -a -G sudo ofe

INSTALL raku from debian/Ubuntu repositories

root@bookworm\~# apt install raku && exit

OR INSTALL raku from source in case of Debian 11 (build instructions: https://rakudo.org/downloads/rakudo/source)

root@bookworm\~# apt-get install curl build-essential libssl-dev

root@bookworm\~# install -d /opt/rakudo/src && cd #_ \
root@bookworm\~# curl -LJO https://rakudo.org/latest/rakudo/src \
root@bookworm\~# tar -xvzf rakudo-*.tar.gz \
root@bookworm\~# cd rakudo-* \
root@bookworm\~# perl Configure.pl --backend=moar --gen-moar --prefix=/opt/rakudo --perl6-home=/usr/local/lib/perl6 \
root@bookworm\~# make

If you wish, you can run the tests (OPTIONAL)\
Depending on your machine, they could take over half an hour to run

root@bookworm\~# make test \
root@bookworm\~# make spectest

root@bookworm\~# make install 

root@bookworm\~# ln -s /opt/rakudo/bin/rakudo /usr/local/bin/rakudo \
root@bookworm\~# ln -s /opt/rakudo/bin/raku /usr/local/bin/raku

Install the zef module manager:

root@bookworm\~# cd /opt && git clone https://github.com/ugexe/zef.git && cd zef && raku -Ilib bin/zef install . \
root@bookworm\~# ln -s /usr/local/lib/perl6/site/bin/zef /usr/local/bin/zef

root@bookworm:\~# exit 

ofe@bookworm:\~$ echo "export PATH=/opt/rakudo/bin:/usr/local/lib/perl6/site/bin:\\$PATH" >> ~/.bashrc \
ofe@bookworm:\~$ source ~/.bashrc \
ofe@bookworm:\~$ exit

LOGIN your Guest

ofe@bookworm:\~$ script onefite-install.log \
(it will record the session until you exit) 

ofe@bookworm:\~$ mkdir $HOME/.local && cd $HOME/.local 

ofe@bookworm:\~/.local$ git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine && git branch site && ./INSTALL && onefite service start

Alternative: start the onefite daemon that starts the onefite service after system boot and keeps it running

ofe@bookworm:\~$ git clone https://github.com/fitteia/OneFit-Engine.git && cd $HOME/.local/OneFit-Engine && ./INSTALL  --systemd-daemon && sudo service onefite start

## CERN Lib Minuit

Since version OneFit-Engine version 0.9.0, Minuit is installed from source in a OneFit-Engine parallel folder minuit \
OneFit-Engine/etc/OFE/default/makefile (used to compile the user defined source) is now set to work with the libminuit.a \
installed from source. In order to keep using the debian package version, uncomment the appropriate MINUIT tag in that makefile.\

To (re-)install the cernlib packages both "INSTALL" and "onefite upgrade..." raku scripts  accept option "--cernlib"

## Updates/Upgrades

Login your guest and

ofe@bookworm:\~$ script onefite-upgrade.log \
(it will record the session until you exit after the upgrade finishes)

ofe@bookworm:\~$ onefite upgrade 

or

ofe@bookworm:\~$ onefite upgrade -d

or

ofe@bookworm:\~$ onefite service stop && cd $HOME/.local/OneFit-Engine && ./INSTALL && onefite service start

or

ofe@bookworm:\~$ sudo service onefite stop && cd $HOME/.local/OneFit-Engine && ./INSTALL -d && sudo service onefite start

ofe@bookworm:\~$ exit \
(to finish recording the session)

## OneFit web service

OneFit Engine web service runs on port 8142 by default\
(you can choose another port number, but be aware of firewall issues) 

### Start the web service

ofe@bookworm:\~$ onefite service start

or

ofe@bookworm:\~$ cd $HOME/public_html && nohup onefite start-web-engine &

### Stop the web service

ofe@bookworm:\~$ onefite service stop

or

ofe@bookworm:\~$ onefite stop-web-engine

or

ofe@bookworm:\~$ onefite service PID (or ofe@bookworm:\~$ ps aux | grep onefite) \
ofe@bookworm:\~$ kill -9 \<PID\>

### View web service log

ofe@bookworm:\~$ onefite service log

## INSTALL usage

Usage:
    ./INSTALL [-h|--help]\
    ./INSTALL [--dpkg] [--cpu] [-c|--compile] [-t|--test] [-i|--install] [-p|--inline-perl5] [-g|--enable-gs] \
              [-w|--web-server] [-u|--uninstall] [--to-site] [--bindir] \
    --dpkg         # to install required debian packages\
    --web-server   # to run tasksel install web-server
    --cpu          # cpu [amd64|arm64] defaults to automaic detection based on "uname -a"\
    --compile      # to comlipe and install the OneFit engine core (use [-/c|--no-c|--/compile|--no-compile] to skip compiling)\
    --test         # to run raku module tests (use [--/test|--no-test] to avoid test fase)\
    --install      # to install OneFit-Engine as a raku modules available to all users\
    --uninstall    # to uninstall OneFit-Engine module \
    --inline-perl5 # to install raku module Inline::Perl5\
    --enable-gs    # enable ghostsript in ImageMagick-6 policy.xml\
    --to-site      # by default install raku module to site (use [--/to-site|--no-to-site] otherwise \
    --bindir       # defaults to $*CWD/../bin (use --bindir=/$HOME/bin to set another folder for binaries)
    negated options:\
    ex: use [-/c|--/compile]  or [--no-c|--no-compile] no negate option compile\



### Install examples:

-) complete install to site with sudo privileges

ofe@bookworm:\~$ cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL

-) install packages, raku Inline::Perl5, and set ghostscript policy settings

ofe@bookworm:\~$ cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL -/c -/i -/t

-) compile and install raku module in user account (no sudo privileges are required)

ofe@bookworm:\~$ cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --no-dpkg --no-enable-gs --no-test --no-to-site --no-web-server

-) uninstall from site

ofe@bookworm:\~$ cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --uninstall

-) uninstall from user account

ofe@bookworm:\~$ cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --no-to-site -u

## OneFit Engine paths

ofe@bookworm:\~$ onefite path

ofe@bookworm:\~$ cd $(onefite path --src | awk ´/\//') # to got to src file


## Running examples

Examples of onefite use can e found in the examples folder.

ofe@bookworm:\~$ cd $HOME/public_html && raku $HOME/.local/OneFit-Engine/xx-RUN.me"

Tipically the output will go to example_xx folder.

ofe@bookworm:\~$ ip -4 -br add

to get the IP address

Use a browser to access the http://\<IP\>/~ofe/ folder.

## TRY:
ofe@bookworm:\~$ raku $HOME/.local/OneFit-Engine/06-raku.me

and

ofe@bookworm:\~$ curl -F "file=@example_06/example_06.json" http://\<IP\>/fit

or

ofe@bookworm:\~$ curl -F "file=@example_06/example_06.json" -F "download=zip" http://\<IP\>/fit | tar zxvf -

or

ofe@bookworm:\~$ curl -F "file=@example_06/example_06.json" -F "download=All.pdf" http://\<IP\>/fit > All.pdf

or

ofe@bookworm:\~$ curl -F "file=@example_06/example_06.json" -F "download=example_06.json" -F "username=\<username\>" http://\<IP\>/fit

or

ofe@bookworm:\~$ curl -F "file=@example_06/example_06.json"  http://\<IP\>/convert

or

ofe@bookworm:\~$ curl -F "file=@example_06.sav"  http://\<IP\>/convert

or

ofe@bookworm:\~$ curl -F "file=@C12-60.hdf5" -F "stelar-hdf5=yes" -F "function=Mz(t,a,b,c[0\<0.5],T11[0\<4],T12[0\<4])[-1.5\<1.5] = a\+ b\*c\*exp(-t/T11)\+b*(1-c)*exp(-t/T12)" -F "autox=yes"  -F "logx=yes" http://\<IP:8142\>/fit 

or

ofe@bookworm:\~$ curl -F "file=@C12-60.hdf5" -F "stelar-hdf5=yes" -F "function=Mz(t,a,b,c[0\<0.5],T11[0\<4],T12[0\<4])[-1.5\<1.5] = a\+ b\*c\*exp(-t/T11)\+b*(1-c)*exp(-t/T12)" -F "autox=yes"  -F "logx=yes" http://\<IP:8142\>/fit | awk -F ', ' '/^zone/ {print $4 " " 1/$11 " " 0.05/$11}'

to get just flarmor and R11 and err_R1

For a zip file of compressed data files

ofe@bookworm:\~$ curl -F "file=@filename.zip" -F "stelar-hdf5=no" -F "function=Mz(t,a,b,c[0\<0.5],T11[0\<4],T12[0\<4])[-1.5\<1.5] = a\+ b\*c\*exp(-t/T11)\+b*(1-c)*exp(-t/T12)" -F "autox=yes"  -F "logx=yes" http://\<IP:8142\>/fit

# Working with function aliases

## onefite list aliases

The list of function aliases that are included in the alias.json file installed \
at $HOME/.local/etc/

Local changes will not be overwritten and updates have to be held locally. \
use diff to check what differences you might have, Ex:
prompt> diff $HOME/.local/etc/alias.json $HOME/.local/OneFit-Engine/etc/OFE/default/alias.json

## onefite fit using aliases

To take advantage of aliases pelase consider:\
host_prompt> onefite fit <datafile> alias:"one exponential" \
host_prompt> onefite fit <datafile> a:"two exponentials" --logx \
host_prompt> onefite fit <datafile> "alias: one BPP" \
host_prompt> onefite fit <datafile> "a: two BPP"
 

# OneFit Engine Virtual Machines

## Accessing and controling the onefit-e VM from the host

### Windows host running VirtualBox VM, guest NAT with port forwarding


host_prompt> VBoxManage startvm onefit-e

host_prompt> VBoxManage controlvm onefit-e poweroff

host_prompt> ssh ofe@localhost -P 8122

### Mac OS host running UTM

host_prompt> echo "/Applications/UTM/Contents/MacOS/" | sudo tee /etc/paths.d/50-UTM && exit

host_prompt> utmctl start onefit-e [--disposable]

host_prompt> utmctl stop onefit-e

Host_prompt> export ONEFITE=$(/Applications/UTM/Contents/MacOS/utmctl ip-address onefit-e | awk '/\./ {print $1}')

host_prompt> ssh ofe@$ONEFITE

## Running OneFit-Engine as a standalone service in the backgound on a virtual machine

### Windows running VirtualBox, guest NAT, with port forwarding

host_prompt> VBoxManage startvm onefit-e --type=headless

host_prompt> putty.exe -ssh ofe@localhost -P 8122

host_prompt> curl.exe -F "file=@datafile.name" -F "function=..." -F "autox=yes" -F "autoy=yes" -F "logx=yes" -F "download=zip" http://localhost:8142/fit

host_prompt> .....

host_prompt> VBoxManage controlvm onefit-e poweroff

### MacOS runnig UTM, guest "shared network"

Edit VM settings and remove Display interface (you can get it back when necessary).

host_prompt> echo "/Applications/UTM/Contents/MacOS/" | sudo tee /etc/paths.d/50-UTM && exit

host_prompt> utmctl start onefit-e --disposable

host_prompt> utmctl stop onefit-e

host_prompt> export ONEFITE=$(/Applications/UTM/Contents/MacOS/utmctl ip-address onefit-e | awk '/\./ {print $1}')

host_prompt> ssh ofe@$ONEFITE

_prompt> curl.exe -F "file=@datafile.name" -F "function=..." -F "autox=yes" -F "autoy=yes" -F "logx=yes" -F "download=zip" http://$ONEFITE:8142/fit

host_prompt> ..

host_prompt> utmctl stop onefit-e


# Managing a site git repository in you onefite box

Ex: adding additional models to your fitting engine in $HOME/.local/OneFit-Engine/C/local

Assuming that your OneFit-Engine.git clone repository is in $HOME/.local/OneFit-Engine

### First Time

prompt> cd $HOME/.local/OneFit-Engine/C/local && git checkout -b site

prompt> git config user.email "your email" && git config user.name "your name"

(creates a new branch "site" and check it out)

### Further changes/updates

prompt> cd $HOME/.local/OneFit-Engine/C/local && git checkout site

1. Create new model functions using an existing one as a template.\
2. Edit userlib.h and add your new functions according to existing examples.\
3: Edit Usrelib.i and add the your functions signatures using the existing ones as examples\don't forget that there two planes in the file where the new functions signatures have to be added.
4: Edit the META-C.json in the $HOME/.local/OneFit-Engine/C folder to add your new functions info

prompt> git commit -a && git chekout main && git pull & git merge -m "some info" site && onefite upgrade -d --test

or as the last step

propmt> onefite upgrade -systemd-daemon --merge-info="some info"





