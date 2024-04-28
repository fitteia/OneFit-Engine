# OneFit Engine
The OneFit Engine (onefite) is fitteia's internal fitting core, made available from the command line through an SSH connection. onefite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process

Fresh instalation
For Debian "bookworm"

In one user account:\
su -\
apt install raku git sudo\
mkdir ~/bin ~/Downloads\

git clone https://github.com/fitteia/OneFit-Engine.git

cd $HOME/OneFit-Engine && ./INSTALL help \
Usage:\
    ./INSTALL [-h|--help]\
    ./INSTALL [--dpkg] [--cpu] [-c|--compile] [-t|--test] [-i|--install] [-p|--inline-perl5] [-g|--enable-gs]\
    --dpkg         # to install required debian packages\
    --cpu          # cpu defaults to amd64\
    --compile      # to comlipe and install the OneFit engine core\
    --test         # to run raku module tests\
    --install      # to install OneFit-Engine as a raku modules available to all users\
    --inline-perl5 # to install raku module Inline::Perl5\
    --enable-gs    # enable ghostsript in ImageMagick-6 policy.xml\
    negated options:\
    ex: use [-/c|--/compile]  or [--no-c|--no-compile] no negate option compile\

cd $HOME/OneFit-Engine && git stash && git pull &&  $$ ./INSTALL


