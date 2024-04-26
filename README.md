# OneFit_Engine
The OneFit Engine (OneFit-E) is fitteia's internal fitting core, made available from the command line through an SSH connection. OneFite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process

Fresh instalation
For Debian "bookworm"

In one user account:
mkdir ~/bin ~/Downloads
zef install Inline::Perl5

git clone https://github.com/fitteia/OneFit-Engine.git

cd $HOME/OneFit-Engine $$ .INSTALL help
cd $HOME/OneFit-Engine $$ .INSTALL --dpkg --image

for subsequent updates
sudo zef uninstall OneFit-Engine
cd $HOME/OneFit-Engine $$ .INSTALL --/test


