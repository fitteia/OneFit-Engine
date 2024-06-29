# OneFit Engine
The OneFit Engine (onefite) is fitteia's internal fitting core, made available from the command line through an SSH connection. onefite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process

## Fresh instalation

OneFit  Engine package is prepared for instalation in Debian

### Windows

Install Oracle VM VirtualBox in your Windows host.\
Download Debian netinst iso file for amd64\
Created a new guest VM with at least 2 cores, 8 GB HD and 4 GB of RAM\
Edit the Network settings and for a NAT network, in Advanced settings -> Port Forwarding add\
port 8022 to localhost and 22 to guest (for ssh connections using puTTY)\
port 8080 to localhost and 80 to guest (for html connections)\
port 8142 to localhost and 8142 for guest (for OneFit-Engine connection using html)\

In the Host: ssh user@localhost -P 8022; http://localhost:8080; http://localhost:8142

### Mac M2

Install UTM\
Download Debian for arm64\
Created a new guest Emulate VM for Architecture ARM64 (aarch64) with at least 8 GB HD and 4 GB of RAM, default number of cores

In the Host: ssh user@192.168.64.11; http://192.168.64.11; http://192.168.64.11:8142

### Debian Linux

For Debian "bookworm"

Suggestion: minimal installation; when at tasksel stage select "SSH server" and "Web server" ("Laptop" might be an option) all other packages will be installed later.\
Create a user account, ex: ofe

#### First login with username ofe

prompt> su -\
prompt> apt install raku git sudo\
prompt> usermod -a -G sudo ofe \
prompt> exit \
promot> logout and login again\
prompt> mkdir $HOME/.local && cd $HOME/.local

prompt> git clone https://github.com/fitteia/OneFit-Engine.git

prompt> cd $HOME/.local/OneFit-Engine && ./INSTALL

prompt> onefite service start


## Updates/Upgrades

Login your guest and

prompt> onefite upgrade

or

prompt> cd $HOME/.local/OneFit-Engine && ./INSTALL 


## OneFit web service

OneFit Engine web service runs on port 8142 by default\
(you can choose another port number, but be aware of firewall issues) 

### Start the web service

prompt> onefite service start

### Stop the web service

prompt> onefite service stop

### View web service log

propmt> onefite service log


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

prompt> cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL

-) install packages, raku Inline::Perl5, and set ghostscript policy settings

prompt> cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL -/c -/i -/t

-) compile and install raku module in user account (no sudo privileges are required)

prompt> cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --no-dpkg --no-enable-gs --no-test --no-to-site

-) uninstall from site

prompt> cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --uninstall

-) uninstall from user account

prompt> cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --no-to-site -u


## Running examples

Examples of onefite use can e found in the examples folder.

prompt> cd $HOME/public_html && raku $HOME/.local/OneFit-Engine/xx-RUN.me"

Tipically the output will go to example_xx folder.

prompt> ip -4 -br add

to get the IP address

Use a browser to access the http://\<IP\>/~ofe/ folder.


## START the onefite-web-engine:
prompt> cd $HOME/public_html && nohup onefite start-web-engine &

## STOP web-engine:
prompt> onefite stop-web-engine

## TRY:
prompt> raku $HOME/.local/OneFit-Engine/06-raku.me
prompt> curl -F "file=@example_06/example_06.json" http://\<IP\>/fit

or

prompt> prompt> curl -F "file=@example_06/example_06.json" -F "download=zip" http://\<IP\>/fit | tar zxvf -

or

prompt> prompt> curl -F "file=@example_06/example_06.json" -F "download=All.pdf" http://\<IP\>/fit > All.pdf

or

prompt> prompt> curl -F "file=@example_06/example_06.json" -F "download=example_06.json" -F "username=\<username\>" http://\<IP\>/fit


