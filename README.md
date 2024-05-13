# OneFit Engine
The OneFit Engine (onefite) is fitteia's internal fitting core, made available from the command line through an SSH connection. onefite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process

Fresh instalation
For Debian "bookworm"

Create a user account (ex: ofe):\
prompt> su -\
prompt> apt install raku git sudo\
prompt> usermod -a -G sudo ofe \
prompt> exit \
prompt> mkdir $HOME/.local && cd $HOME/.local

prompt> git clone https://github.com/fitteia/OneFit-Engine.git

prompt> cd $HOME/.local/OneFit-Engine && ./INSTALL help \
Usage:
    ./INSTALL [-h|--help]\
    ./INSTALL [--dpkg] [--cpu] [-c|--compile] [-t|--test] [-i|--install] [-p|--inline-perl5] [-g|--enable-gs]\
    --dpkg         # to install required debian packages\
    --web-server   # to run tasksel install web-server
    --cpu          # cpu [amd64|arm64] defaults to automaic detection based on "uname -a"\
    --compile      # to comlipe and install the OneFit engine core (use [-/c|--no-c|--/compile|--no-compile] to skip compiling)\
    --test         # to run raku module tests (use [--/test|--no-test] to avoid test fase)\
    --install      # to install OneFit-Engine as a raku modules available to all users\
    --inline-perl5 # to install raku module Inline::Perl5\
    --enable-gs    # enable ghostsript in ImageMagick-6 policy.xml\
    --to-site      # by default install raku module to site (use [--/to-site|--no-to-site] otherwise \
    --bindir       # defaults to $*CWD/../bin (use --bindir=/$HOME/bin to set another folder for binaries)
    negated options:\
    ex: use [-/c|--/compile]  or [--no-c|--no-compile] no negate option compile\

prompt> cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL


Examples:

Examples of onefite use can e found in the examples folder.

prompt> cd $HOME/public_html && raku xx-RUN.me"

Tipically the output will go to example_xx folder.

prompt> ip -4 -br add

to get the IP address

Use a browser to access the http://\<IP\>/~ofe/ folder.
