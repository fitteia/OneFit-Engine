# OneFit Engine
The OneFit Engine (onefite) is fitteia's internal fitting core, made available from the command line through an SSH connection. onefite is intended to run in a virtual machine, either locally or remotely, facilitating the embedding of fitteia's fitting core in user-defined environments and enabling the personalization of graphical user interfaces.

The copyright will be defined at the end of the devolopment process. For now the we use the Artistic License 2.0.

## Table of Contents

- [Prerequisites](#Prerequisites)
- [Installation](#Installation)
- [Performance](#Performance)
- [Upgrades](#Upgrades)
- [WebService](#WebService)
- [INSTALL-help](#INSTALL-help)
- [Data-file-types](#Data-file-types)
- [Running](#Running)
- [Usage](#Usage)

## Prerequisites

### Simple single user instalations

 ** Windows WSL2
	
Install WSL debian
```powershell
wsl --install -d debian
```
Set default to WSL2
```powershell
wsl --set-default-version 2
```
 ** MacOS Colima+Docker

Install Colima and Docker with brew
```bash
brew install colima docker docker-buildx
mkdir -p ~/.docker/cli-plugins
ln -sf "$(brew --prefix)/opt/docker-buildx/bin/docker-buildx" \
  ~/.docker/cli-plugins/docker-buildx
```
Start Colima with 4GB or RAM (less will not work)
```bash
/opt/homebrew/opt/colima/bin/colima start --memory 4 --runtime docker  
```
 ** MacOS Native

A litle bit of hacking might be necessary but...start with brew
```bash
brew install rakudo gcc
```
A litle bit of hacking might be necessary. Install raku zef module.
```bash
mkdir ~/.raku $$ cd $_ && git clone https://github.com/ugexe/zef.git && cd zef && raku -I. bin/zef install . && ln -s /opt/homebrew/share/perl6/site/bin/zef /usr/local/bin/zef
```

 ** Fedora/CentOS/Suse/Arch
```bash
which git
```
Install git in your system and then
```bash
mkdir -p $HOME/.local && cd $_
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine
./pre-install-ofe-in-Linux.sh && ./INSTALL --no-test && ./post-install
```

### Full features OneFit Engine Server

 ** Windows: **

- [VirtualBox](https://www.virtualbox.org)
- [FilleZilla](https://filezilla-project.org) (suggestion)
- [Curl](https://curl.se/windows) (suggestion)

	Install Oracle VM VirtualBox in your Windows host.\
	Download Debian netinst iso file for amd64\
	Creat a new guest VM with at least 4 cores, 6 GB HD and 4 GB of RAM\
	(ATTENTION: select "Skip the Unattended Guest Installation" to avoid a full debian installation)  
	Edit the Network settings and for a NAT network, in Advanced settings -> Port Forwarding add\
	port 8122 to localhost and 22 to guest (for ssh connections using puTTY)\
	port 8180 to localhost and 80 to guest (for html connections)\
	port 8142 to localhost and 8142 for guest (for OneFit-Engine connection using html)\
	port 8100 to localhost and 8100 for guest (if you want to use the a web based shell, access)

	In the Host: ssh user@localhost -P 8122; http://localhost:8180; http://localhost:8142

	When using FileZilla use sftp://localhost:8122/ for encripted acces with ssh 

 ** Mac M2: **

- [UTM](https://mac.getutm.app)

	Download Debian for arm64\
	Created a new guest Virtualize VM for Linux Architecture ARM64 (aarch64) with at least 8 GB HD, 4 GB of RAM,
	default number of cores.

	In the Host: ssh user@192.168.64.11; http://192.168.64.11; http://192.168.64.11:8142


## Installation

During installation, users can set the maximum number of fitting parameters with:

	./INSTALL ... --minuit=###

	The default value is 200.

	Aliases for the default onefite program name can be defined during installation with:

	./INSTALL ... --alias "ofe onefit of"

	or any other list of aliases.
	After installation, users can also create additional shell commands at any time 
	to simplify repetitive or routine tasks.

```bash
which ofe
unset -f ofe
ofe() { command onefite "$@"; }

which iofe
unset -f iofe
onefiteSrcPath="$(onefite path --src 2>/dev/null)"
iofe() {
    cd "$onefiteSrcPath"
    ./INSTALL --no-test
    cd -
}
```

### Simple single user instalations

 ** Windows WSL2
	
Start debian
```powershell
wsl -d debian
```
In debian shell
```bash
sudo apt install raku git
```
In the following commands, replace "ofe" by the username you choosed when you installed wsl
```bash
mkdir /home/ofe/public_html /home/ofe/.local && cd $_ && git clone https://github.com/fitteia/OneFit-Engine.git
```
```bash  
cd /home/ofe/.local/OneFit-Engine && ./INSTALL --wsl && onefite service start --ip=127.0.0.1 && onefite test 
```

 ** MacOS Colima+Docker

Get the Dockerfile from https://github.com/fitteia/OneFit-Engine/blob/dev/Dockerfile and build the container
```bash
curl -O https://raw.githubusercontent.com/fitteia/OneFit-Engine/dev/Dockerfile && docker build -t onefite .
```
Run the onefite container
```bash
docker run -e UID=$(id -u) -e GID=$(id -g) -p 8142:8142 -it --mount type=bind,source=/Users/$(id -nu)/Docker,target=/home/ofe/public_html  --name onefite onefite bash
```
or
```bash
docker run -d -e UID=$(id -u) -e GID=$(id -g) -p 8142:8142 -it --mount type=bind,source=/Users/$(id -nu)/Docker,target=/home/ofe/public_html  --name onefite onefite bash
docker exec -it onefite bash
```
In the container
```bash
cd /home/ofe/public_html && onefite service start && onefite test
```
 ** MacOS Native

Clone OneFit-Engine
```bash
mkdir ~/folder && cd $_ && git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine && ./INSTALL  --macos
```

Make sure zef and App::Prove6 are installed and links for them exist in /usr/local/bin.

If the links are missing, or do not point to the most recente version of rakudo,
```bash
cd /usr/local/bin
sudo ln -s "$(raku -e 'say $*REPO.repo-chain[1].Str.subst("inst#","")')/bin/zef"
sudo ln -s "$(raku -e 'say $*REPO.repo-chain[1].Str.subst("inst#","")')/bin/prove6"
```

Start the OneFit-Engine service
```bash
onefite service start --ip=127.0.0.1
```
Run tests
```bash
onefite test --no-system-daemon 
```



## Detailed installation

OneFit  Engine package is prepared for instalation in Debian

** APT based Linux distros (Debian, Ubuntu, Linux Mint,...) **

Ex: Debian 12 "bookworm" and Ubuntu 24.04.1 LTS "Noble Numbat"

Suggestion: minimal installation; when at tasksel stage select "SSH server" and "Web server" 
("Laptop" might be an option) all other packages will be installed later.	

Create a user account, ex: ofe

1.	**First login with username ofe:**

	```bash
	su - 
	```
	```bash
	apt install git sudo && usermod -a -G sudo ofe
	```
	INSTALL raku from debian/Ubuntu repositories
	
	```bash
	apt install raku 
	```

	Aternatively: install raku from source in case of Debian 11 (build instructions: https://rakudo.org/downloads/rakudo/source)
	```bash
	apt-get install curl build-essential libssl-dev
	install -d /opt/rakudo/src && cd $_ 
	curl -LJO https://rakudo.org/latest/rakudo/src 
	tar -xvzf rakudo-*.tar.gz 
	cd rakudo-* 
	perl Configure.pl --backends=moar --gen-moar --prefix=/opt/rakudo --perl6-home=/usr/local/lib/perl6 
	make
	```
	If you wish, you can run the tests (OPTIONAL)\
	Depending on your machine, they could take over half an hour to run
	```bash
	make test 
	make spectest
	make install 
	ln -s /opt/rakudo/bin/rakudo /usr/local/bin/rakudo 
	ln -s /opt/rakudo/bin/raku /usr/local/bin/raku
    ```

	Install the zef module manager:
	```bash
	cd /opt && git clone https://github.com/ugexe/zef.git 
	cd zef && raku -Ilib bin/zef install . 
	ln -s /usr/local/lib/perl6/site/bin/zef /usr/local/bin/zef
	```
	Exit superuser mode
	```bash
	exit
	```

	Add folder to PATH
	```bash
	echo "export PATH=/opt/rakudo/bin:/usr/local/lib/perl6/site/bin:\\$PATH" >> ~/.bashrc \
	source ~/.bashrc \
	exit
	```

2. **LOGIN your Guest**

	Set script to record your installation log
	```bash 
	script onefite-install.log 
	```

	(it will record the session until you exit). remove the --no-test if you wnat all tests to be performed during installation. Otherwise consider running "onefite test" after installation. 

	```bash
	mkdir $HOME/.local && cd $HOME/.local 
	git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine && git branch site && ./INSTALL --no-test && onefite service start
	```

	Alternative: start the onefite daemon that starts the onefite service after system boot and keeps it running
	```bash
	git clone https://github.com/fitteia/OneFit-Engine.git && cd $HOME/.local/OneFit-Engine && ./INSTALL  --systemd-daemon --no-test && onefite service start
	```

	Alternative: install also a web shell access (linuxinabox) and start the onefite daemon that starts the onefite service after system boot and keeps it running
	```bash
	git clone https://github.com/fitteia/OneFit-Engine.git && cd $HOME/.local/OneFit-Engine && ./INSTALL  --systemd-daemon --shell --no-test && onefite service start
	```

	***CERN Lib Minuit***

	Since version OneFit-Engine version 0.9.0, Minuit is installed from source, for a maximum of 200 fitting parameters (otherwise ./INSTALL....--minuit=###) , in a OneFit-Engine parallel folder minuit OneFit-Engine/etc/OFE/default/makefile (used to compile the user defined source) is now set to work with the libminuit.a installed from source. In order to keep using the debian package version, uncomment the appropriate MINUIT tag in that makefile.

	To (re-)install the cernlib packages both "INSTALL" and "onefite upgrade..." raku scripts  accept option "--cernlib"

	```bash
	service shellinabox stop 
	```
	(to stop serving shell acces through port 8100 .
	```bash		
	sudo apt purge shelloinabox 
	```
	(to remove shellinabox from the system)

## Performance

OFE provides a reasonable level of parallelization, particularly when performing individual fits.

In the case of MIXED fits—where some model parameters are obtained from a global χ² minimization target and others are obtained from individual data set χ² minimizations—OFE offers a second level of parallelization. The performance depends on the number of available CPU cores. The user can control the level of parallelization using the --workers=Int option, or disable parallelization with the --no-parallel option.

Some MIXED fits can become I/O-intensive. In such cases, it is beneficial to perform the fits on a RAM disk. For Debian/Ubuntu, WSL (Debian), and macOS setups using Colima/Docker/Debian, a RAM-backed filesystem is typically available at /dev/shm .

When using OFE through the web interface, all fits are performed in the folder $HOME/public_html. In that case, after completing the installation, you can run:

```bash
mv $HOME/public_html/* /dev/shm
rm -fr $HOME/public_html
ln -s /dev/shm $HOME/public_html
``` 

This may result in a significant reduction in runtime for some fits.

## Upgrades

use option --no-test or --/test to void runnig the installation tests

Login your guest and set script to record your upgrade log
```bash
script onefite-upgrade.log 
onefite upgrade 
```

Alternatively, upgrade and install systemd-daemon:
```bash
onefite upgrade -d
```

Alternatively, do it by hand:
```bash
onefite service stop && cd $HOME/.local/OneFit-Engine && ./INSTALL && onefite service start
```

Alternatively, for the systemd-daemon:
```bash
sudo service onefite stop && cd $HOME/.local/OneFit-Engine && ./INSTALL -d && sudo service onefite start
```

Finally, to finish recording:
```bash
exit 
```

## WebServicea

OneFit Engine web service runs on port 8142 by default (you can choose another port number, but be aware of firewall issues) 

**Start the web service:**
```bash
onefite service start
```
or
```bash
cd $HOME/public_html && nohup onefite start-web-engine &
```

**Stop the web service:**
```bash
onefite service stop
```
or
```bash
onefite stop-web-engine
```
or
```bash
kill -9 "$(ps aux | awk '/onefite/ && !/awk/ {print $2}')" 
```

**View web service log:**
```bash
onefite service log
```

Shell acces via Web start/stop
```bash
service shellinabox stop
```

## INSTALL-help

 **Usage:**

	./INSTALL [--dpkg] [-w|--web-server] [--cpu=<Str>] [-c|--compile] [-t|--test] [-i|--install] [-U|--Uninstall] [-p|--p6-modules] [-g|--enable-gs] [-s|--to-site] [-u|--to-user] [-d|--systemd-daemon] [--cernlib] [--shell] [--git] [--wsl] [--macos] [--docker] [--post-test] [--man-page] [--ip=<Str>] [--port[=Int]] [--minuit[=Int]] [--shell-port[=Int]] [-b|--bindir=<Str>] [-m|--merge-with|--merge-from|--merge-site=<Str>] -- onefite INSTALL script
  	./INSTALL help -- onefite INSTALL help
  	./INSTALL [-h|--help] -- onefite INSTALL help
  
    	--dpkg                                             to install required debian packages [default: True]
    	-w|--web-server                                    to run tasksel install web-server [default: True]
    	--cpu=<Str>                                        [amd64|arm64| defaults to "uname -a" detection
    	-c|--compile                                       to compile and install the OneFit engine core [default: True]
    	-t|--test                                          to run raku module tests [default: True]
    	-i|--install                                       to install OneFit-Engine as a raku modules available to all users [default: True]
    	-U|--Uninstall                                     to uninstall OneFit-Engine module [default: False]
    	-p|--p6-modules                                    attempt to install raku module Inline::Perl5 [default: True]
    	-g|--enable-gs                                     enable ghostscript in ImageMagick policy.xml [default: True]
    	-s|--to-site                                       install raku module to site (use --/to-site to install-to user) [default: True]
    	-u|--to-user                                       if True --/dpkg -/w -/g --no-to-site [default: False]
    	-d|--systemd-daemon                                if true runs sudo systemctl start onefite (or sudo service onefite start) [default: False]
    	--cernlib                                          by default it NOT will not install the cernlib packages (Later the user has to modify the ./OneFit-Engine/etc/OFE/default/makefile MINUIT variable settings [default: False]
    	--shell                                            by default it NOT will install shellinabox service in port 8100 [default: False]
    	--git                                              By default it runs git to pull the package for remote repository [default: True]
    	--wsl                                              Flag for a WSL2 Install ( --/test --/post-test --/web-server --/system-daemon --/dpkg ) [default: False]
    	--macos                                            Flag for a MacOS Install ( --/test --/post-test --/web-server --/system-daemon --/dpkg ) [default: False]
    	--docker                                           Flag for a Docker Install ( --/test --/post-test --/web-server --/system-daemon --/dpkg ) [default: False]
    	--post-test                                        Run post install tests [default: True]
    	--man-page                                         Install man page [default: True]
    	--ip=<Str>                                         accept access from IP [default: '*']
    	--port[=Int]                                       onefite service port [default: 8142]
    	--minuit[=Int]                                     Minuit maximum number of parameters: defualt=50 [default: 200]
    	--shell-port[=Int]                                 shellinabox service port [default: 8100]
    	-b|--bindir=<Str>                                  choose /home/<user>/bin if you want and make sure it is included in the PATH [default: '/Users/pedros/Downl…']
    	-m|--merge-with|--merge-from|--merge-site=<Str>    if merge string then merge site branch (it it exits) into current branch [default: '']
    	-h|--help                                          help
   	
		negated options:\
    	ex: use [-/c|--/compile]  or [--no-c|--no-compile] to negate option compile\


-   **Install examples:**

	complete install to site with sudo privileges
	
	```bash
	cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL
	```

	install packages, raku Inline::Perl5, and set ghostscript policy settings

	```bash
	cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL -/c -/i -/t
	```

	compile and install raku module in user account (no sudo privileges are required)
	
	```bash
	cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --no-dpkg --no-enable-gs --no-test --no-to-site --no-web-server
	```

	uninstall from site

	```bash
	cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --uninstall
	```

	uninstall from user account

	```bash
	cd $HOME/.local/OneFit-Engine && git stash && git pull && ./INSTALL --no-to-site -u
	```

- 	**OneFit Engine paths:**

	```bash
	onefite path
	```

	```bash
	cd $(onefite path --src) 
	```

## Data-file-types

### Standard

- Ascii files with two (or three) columns separated by spaces: x y or (z y ey)

### Specific

- Stelar sef file: call onefite with option --sef-R1-file=filename 

### ZIP files

zip files containg any of the above 

## Running

Define a bash function rune

``bash
rune() { cd $HOME/public_html && raku $HOME/.local/OneFit-Engine/examples/command-line/$1.me; }
```

Examples of onefite user can found in the examples folder $HOME/.local/OneFit-Engine/examples/command-line/

```bash
ls $HOME/.local/OneFit-Engine/examples/command-line/
```

rune the first example and continue with the remaning examples

```bash
rune 01-raku
```
***running via curl***

In your host (in the case of a server running in a virtual machine)
start a web browser, check your server IP, and access it using the
URL http://\<IP\>/~ofe/ 

This will allow you to monitor your tests

****server side****

```bash
ip -4 -br add
cd $HOME/public_html && mkdir ofe && raku $HOME/.local/OneFit-Engine/examples/command-line/06.raku.me
curl -F "file=@example_06/example_06.json" http://127.0.0.1:8142/fit
curl -F "file=@example_06/example_06.json" -F "download=zip" http://127.0.0.1:8142/fit -s --output tmp.zip && unzip tmp.zip && rm tmp.zip
curl -F "file=@example_06/example_06.json" -F "download=All.pdf" http://127.0.0.1:8142/fit -s --output All.pdf
curl -F "file=@example_06/example_06.json" -F "download=example_06.json" -F "username=ofe" -s http://127.0.0.1:8142/fit
curl -F "file=@example_06/example_06.json" -F "download=example_06.json" -s http://127.0.0.1:8142/fit/ofe
curl -F "file=@example_06/example_06.json"  http://127.0.0.1:8142/convert -s > example_06.sav
curl -F "file=@example_06.sav"  http://127.0.0.1:8142/convert -s
```
More examples:

```bash
cd $HOME/public_html && unzip $HOME/.local/OneFit-Engine/examples/command-line/test-data/C12-60.zip

curl http://127.0.0.1:8142/fit/			\
	-F "file=@C12-60.hdf5" 			\
	-F "stelar-hdf5=yes" 			\
	-F "function=				\
		Mz [-1.5<1.5] (			\
			t[1e-3<10],		\
			a,			\
			b,			\
			c=1 [0.5<1],		\
			T11:0.05 [0<4],		\
			T12:0.01 [0<4]		\
		)=				\
		a			\+	\
		b*c*exp(-t/T11)		\+	\
		b*(1-c)*exp(-t/T12)" 		\
	-F "autox=yes"				\
	-F "autoy=yes"				\
	-F "logx=yes"				\
	--silent
```

Alternatively:

```bash
cd $HOME/public_html && unzip $HOME/.local/OneFit-Engine/examples/command-line/test-data/C12-60.zip

curl http://127.0.0.1:8142/fit/ofe -F "file=@C12-60.hdf5" -F "stelar-hdf5=yes" -F "function=Mz(t[1e-3<10],a,b,c=1[0.5<1],T11:0.05[0<4],T12:0.05[0<4])[-1.5<1.5] = a\+ b*c*exp(-t/T11)\+b*(1-c)*exp(-t/T12)" -F "autox=yes"  -F "autox=yes" -F "logx=yes" | awk -F ', ' '/^zone/ {print $4 " " 1/$11 " " sqrt($3)*$12/$11/$11}'
```

to get just Larmor frequency and R11 and err_R1

For a zip file of compressed data files

```bash
cd $HOME/public_html/ofe/C12-60 && zip filename.zip zone*.dat 

curl http://127.0.0.1:8142/fit -F "file=@filename.zip" -F "stelar-hdf5=no" -F "function=Mz(t,a,b,c[0<0.5],T11[0<4],T12[0<4])[-1.5<1.5] = a\+ b*c*exp(-t/T11)\+b*(1-c)*exp(-t/T12)" -F "autox=yes"  -F "logx=yes"
```

# Working with function aliases

## onefite list aliases

The list of function aliases that are included in the aliases.json file installed \
at $HOME/.local/etc/

Local changes will not be overwritten and updates have to be held locally. \
use diff to check what differences you might have, Ex:
prompt> diff $HOME/.local/etc/aliases.json $HOME/.local/OneFit-Engine/etc/OFE/default/aliases.json

## onefite fit using aliases

To take advantage of aliases pelase consider:
```bash
onefite fit alias:"one exponential" <datafile> 
```
```bash
onefite fit a:"two exponentials" <datafile> --logx 
```
```bash
onefite fit "alias: one BPP" <datafile> 
```
```bash
onefite fit "a: two BPP" <datafile> 
```
 

# OneFit Engine Virtual Machines

## Accessing and controling the onefit-e VM from the host

### Windows host running VirtualBox VM, guest NAT with port forwarding


```bash
host_prompt> VBoxManage startvm onefit-e
```
```bash
host_prompt> VBoxManage controlvm onefit-e poweroff

```

```bash
ssh ofe@localhost -P 8122
```
### Mac OS host running UTM

```bash
echo "/Applications/UTM/Contents/MacOS/" | sudo tee /etc/paths.d/50-UTM && exit
```
```bash
utmctl start onefit-e [--disposable]
```
```bash
utmctl stop onefit-e
```
```bash
export ONEFITE=$(/Applications/UTM/Contents/MacOS/utmctl ip-address onefit-e | awk '/\./ {print $1}')
```
```bash
ssh ofe@$ONEFITE
```
## Running OneFit-Engine as a standalone service in the backgound on a virtual machine

### Windows running VirtualBox, guest NAT, with port forwarding

```bash
VBoxManage startvm onefit-e --type=headless
```
```bash
putty.exe -ssh ofe@localhost -P 8122
```
```bash
curl.exe -F "file=@datafile.name" -F "function=..." -F "autox=yes" -F "autoy=yes" -F "logx=yes" -F "download=zip" http://localhost:8142/fit
```

.....


```bash
VBoxManage controlvm onefit-e poweroff
```
### MacOS runnig UTM, guest "shared network"

Edit VM settings and remove Display interface (you can get it back when necessary).

```bash
echo "/Applications/UTM/Contents/MacOS/" | sudo tee /etc/paths.d/50-UTM && exit
```
```bash
utmctl start onefit-e --disposable
```
```bash
utmctl stop onefit-e
```
```bash
export ONEFITE=$(/Applications/UTM/Contents/MacOS/utmctl ip-address onefit-e | awk '/\./ {print $1}')
```
```bash
ssh ofe@$ONEFITE
```
```bash
curl.exe -F "file=@datafile.name" -F "function=..." -F "autox=yes" -F "autoy=yes" -F "logx=yes" -F "download=zip" http://$ONEFITE:8142/fit
```
....

```bash
utmctl stop onefit-e
```

# Managing a site git repository in you onefite box

Ex: adding additional models to your fitting engine in $HOME/.local/OneFit-Engine/C/local

Assuming that your OneFit-Engine.git clone repository is in $HOME/.local/OneFit-Engine

### First Time

```bash
cd $HOME/.local/OneFit-Engine/C/local && git checkout -b site
```
```bash
git config user.email "your email" && git config user.name "your name"
```
(creates a new branch "site" and check it out)

### Further changes/updates

```bash
cd $HOME/.local/OneFit-Engine/C/local && git checkout site
```
1. Create new model functions using an existing one as a template.\
2. Edit userlib.h and add your new functions according to existing examples.\
3: Edit Usrelib.i and add the your functions signatures using the existing ones as examples\don't forget that there two planes in the file where the new functions signatures have to be added.
4: Edit the META-C.json in the $HOME/.local/OneFit-Engine/C folder to add your new functions info

```bash
git commit -a && git chekout main && git pull & git merge -m "some info" site && onefite upgrade -d --test
```
or as the last step

```bash
onefite upgrade -systemd-daemon --merge-site=site
```

## Usage

```bash
Usage:
  onefite [-h|--help] -- Get onefite usage help
  onefite [--man] -- Get onefite man page
  onefite archive [-f|--fit=<Str>] [--<_>=...] -- Show archive log
  onefite ar [-f|--fit=<Str>] [--<_>=...] -- Show archive log
  onefite log [-f|--fit=<Str>] [--<_>=...] -- Show archive log
  onefite man [-m|--man] -- Get onefite man page
  onefite list [-q|--quiet] -- Get onefite library models and aliases lists
  onefite list models [-q|--quiet] -- Get onefite library models list
  onefite list aliases [-q|--quiet] -- Get onefite library models aliases list
  onefite convert <input-file> <output-file> [--wf|--work-folder=<Str>] -- onefite convert sav <-> json files
  onefite path [-s|--src] [-t] [--exa|--examples] [-b|--bin] [-l|--lib] [-c|-C] [--ar|--archive] [--log] [-a|--aliases] -- Get onefite paths to src, etc
  onefite upgrade [-u|--to-user] [-c|--compile] [-d|--systemd-daemon] [--cernlib] [--shell] [--git] [--shell-port[=Int]] [--ip=<Str>] [--wsl] [--macos] [--docker] [-p|--port[=Int]] [--test] [--apt|--apt-get|--dpkg] [-m|--merge-from|--merge-with|--merge-site=<Str>] -- onefite upgrade
  onefite uninstall -- onefite uninstall
  onefite service start [-d|--systemd-daemon] [--rd|--ram|--ramdisk|--RAMDisk|--use-ramdisk] [--ip=<Str>] [-p|--port[=Int]] -- onefite web service start
  onefite service stop -- onefite web service stop
  onefite service log -- onefite web service log
  onefite service PID -- get onefite service PID
  onefite stop-web-engine -- onefite web service stop by removing service process
  onefite start-web-engine [--ip[=Any]] [-p|--port[=Any]] -- onefite web engine
  onefite help <model> [-q|--quiet] -- onefite help on models
  onefite help <model> <key> [-q|--quiet] -- onefite help on model and keys
  onefite start-engine [--np|--no-plot] [-q|--quiet] [--wf|--work-folder=<Str>]
  onefite create <function> [<data-files> ...] [--dl|--data-label|--data-labels=<Str>] [--fm|--fit-methods=<Str>] [-t|--tags=<Str>] [--ax|--auto-x|--autox] [--ay|--auto-y|--autoy] [--axy|--auto-xy|--autoxy] [--lx|--logx|--log-x|--xlog|--loglin] [--ly|--logy|--log-y|--ylog|--linlog] [--lxy|--logxy|--log-xy|--xylog|--loglog] [-g|--global] [--ist-ffc-Mz|--ist-ffc] [--fi|--fit-if=<Str>] [--pi|--plot-if=<Str>] [--sf|--sef-R1-file=<Str>] [-r|--range=<Str>] [--R1] [--gfilt[=Int]] [--se|--set-err|--err=<Str>] [--SymbSize|--ssz[=Str where { ... }]] [--st|--save-to|--to=<Str>] [--wf|--work-folder=<Str>] [-n|--Num|--npts[=Int]] [-q|--quiet] [--ac|--AC|--AuxCode|--auxcode|--aux-code=<Str>] [--<_>=...] -- onefite create a json file from cmd line
  onefite random <function> [<data-files> ...] [--dl|--data-label|--data-labels=<Str>] [-t|--tags=<Str>] [--zt|--zip-to=<Str>] [--inp|--no-plot] [--ax|--auto-x|--autox] [--ay|--auto-y|--autoy] [--axy|--auto-xy|--autoxy] [--lx|--logx|--log-x|--xlog|--loglin] [--ly|--logy|--log-y|--ylog|--linlog] [--lxy|--logxy|--log-xy|--xylog|--loglog] [-g|--global] [--st|--to|--save-to=<Str>] [--wf|--work-folder=<Str>] [-n|--Num|--npts[=Int]] [-q|--quiet]
  onefite fit <function> [<data-files> ...] [--data-label|--data-labels=<Str>] [--fm|--fit-methods=<Str>] [-t|--tags=<Str>] [--zt|--zip-to=<Str>] [--np|--no-plot] [--ax|--auto-x|--autox] [--ay|--auto-y|--autoy] [--axy|--auto-xy|--autoxy] [--lx|--logx|--log-x|--xlog|--loglin] [--ly|--logy|--log-y|--ylog|--linlog] [--lxy|--logxy|--log-xy|--xylog|--loglog] [-g|--global] [--hybrid] [--fi|--fit-if=<Str>] [--pi|--plot-if=<Str>] [--sf|--sef-R1-file=<Str>] [-r|--range=<Str>] [--R1] [--rc|--rchi2|--reduced-chi2] [--gfilt[=Int]] [--se|--set-err|--err=<Str>] [--SymbSize|--ssz[=Str where { ... }]] [--st|--save-to=<Str>] [--wf|--work-folder=<Str>] [-n|--num|-N|--Num|--npts[=Int]] [--ar|--archive] [--eb|--error-bars|--errorbars] [-q|--quiet] [--mp4|--mpeg4] [--rd|--ram|--ramdisk|--RAMDisk|--use-ramdisk] [--parallel] [--workers[=Int]] [--da|--define-alias|--dali=<Str>] [--ac|--AC|--AuxCode|--auxcode|--aux-code=<Str>] [--ro|--remove-outliers=<Str>] [--pc|--prco|--print-cols|--print-columns|--cols=<Str>] [--pco|--peco|--pearson-correlation|--R2] [--<_>=...]
  onefite fit <input-file> [--fm|--fit-methods=<Str>] [--np|--no-plot] [--ax|--auto-x|--autox] [--ay|--auto-y|--autoy] [--axy|--auto-xy|--autoxy] [--lx|--logx|--log-x|--xlog|--loglin] [--ly|--logy|--log-y|--ylog|--linlog] [--lxy|--logxy|--log-xy|--xylog|--loglog] [--st|--save-to|--to=<Str>] [--rc|--rchi2|--reduced-chi2] [--zt|--zip-to=<Str>] [--wf|--work-folder=<Str>] [-n|--num|-N|--Num|--npts[=Int]] [-e|--export] [--eb|--error-bars|--errorbars] [-q|--quiet] [--mp4|--mpeg4] [--rd|--ram|--ramdisk|--RAMDisk|--use-ramdisk] [--ro|--remove-outliers=<Str>] [--da|--define-alias|--dali=<Str>] [--ar|--archive] [--pc|--prco|--print-cols|--print-columns|--cols=<Str>] [--pco|--peco|--pearson-correlation|--R2] [-g|--global] [--hybrid] [-i|--individual] [--parallel] [--workers[=Int]] [--<_>=...]
  onefite plot <input-file> [--ax|--auto-x|--autox] [--ay|--auto-y|--autoy] [--axy|--auto-xy|--autoxy] [--lx|--logx|--log-x|--xlog|--loglin] [--ly|--logy|--log-y|--ylog|--linlog] [--lxy|--logxy|--log-xy|--xylog|--loglog] [--wf|--work-folder=<Str>] [-n|-N|--Num|--npts[=Int]]
  onefite test list
  onefite test [<arr> ...] [-d|--systemd-daemon] [-p|--post-install] [--verbose|-v]
  onefite [-v|--version]

    -h|--help                                                help option true by default
    --man                                                    generate man page standar output [default: False]
    -f|--fit=<Str>                                           fit archive log entry: ex: onefite archive -f last or onefite log --fit 10 [default: '']
    -q|--quiet                                               reduces output log info, false by default [default: False]
    <input-file>                                             name of the file to be converted (sav or json)
    <output-file>                                            name of the converted file (sav or json)
    --wf|--work-folder=<Str>                                 work folder for onefite convert [default: '/tmp']
    -s|--src                                                 path to src [default: False]
    -t                                                       path to test [default: False]
    --exa|--examples                                         path to examples [default: False]
    -b|--bin                                                 path to bin [default: False]
    -l|--lib                                                 path to lib [default: False]
    -c|-C                                                    path to C code [default: False]
    --ar|--archive                                           path to archive [default: False]
    --log                                                    path to log [default: False]
    -u|--to-user                                             install to user [default: False]
    -c|--compile                                             compile before install [default: True]
    -d|--systemd-daemon                                      create a sysmtemd-daemon service [default: False]
    --cernlib                                                install cern lib instead of just minuit.a from src [default: False]
    --shell                                                  install shellinabox [default: False]
    --git                                                    By default it runs git to pull the package for remote repository [default: True]
    --shell-port[=Int]                                       shellinabox port [default: 8100]
    --ip=<Str>                                               set IP [default: '0.0.0.0']
    --wsl                                                    subset of options for a Windows Subsystem Linux [default: False]
    --macos                                                  subset of options for a MacOS native install with brew [default: False]
    --docker                                                 subset of options for a MacOS colima/docker install [default: False]
    -p|--port[=Int]                                          onefite web service port [default: 8142]
    --test                                                   perform tests [default: False]
    --apt|--apt-get|--dpkg                                   run apt update/upgrade [default: True]
    -m|--merge-from|--merge-with|--merge-site=<Str>          merge remote onefite main/dev branches with locl branch [default: '']
    --rd|--ram|--ramdisk|--RAMDisk|--use-ramdisk             run service on a RAMDisk [default: False]
    --ip[=Any]                                               run onefite web service in ip
    -p|--port[=Any]                                          onefite web service port [default: 8142]
    <model>                                                  model name
    <key>                                                    model key info
    --np|--no-plot                                           no plot
    <function>                                               fitting function
    --dl|--data-label|--data-labels=<Str>                    data labels and values
    --fm|--fit-methods=<Str>                                 minuit fit methods [default: 'simp scan min minos']
    -t|--tags=<Str>                                          Data blocks TAGS
    --ax|--auto-x|--autox                                    auto scale x
    --ay|--auto-y|--autoy                                    auto scale y
    --axy|--auto-xy|--autoxy                                 auto scale xy
    --lx|--logx|--log-x|--xlog|--loglin                      logaroithmic x scale
    --ly|--logy|--log-y|--ylog|--linlog                      logarithmic y scale
    --lxy|--logxy|--log-xy|--xylog|--loglog                  log log scale
    -g|--global                                              global/individual [default: False]
    --ist-ffc-Mz|--ist-ffc                                   import Mz data from ist-ffc [default: False]
    --fi|--fit-if=<Str>                                      include fit if condition when fitting data [default: '']
    --pi|--plot-if=<Str>                                     include plot if tag condition when plotting data and fitting curves [default: '']
    --sf|--sef-R1-file=<Str>                                 use file to get freqs when importing data from Stelar sef files [default: '']
    -r|--range=<Str>                                         define range when importing data zones form Stelar data [default: '']
    --R1                                                     import R1 and not Mz data from ist-ffc and Stelar sdf files [default: False]
    --gfilt[=Int]                                            used a Gaussian filter to smooth data
    --se|--set-err|--err=<Str>                               set err in data files doesn't apply to ist-ffc, sef, hdf5 data files [default: '']
    --SymbSize|--ssz[=Str where { ... }]                     set size of symbols [default: '1.0']
    --st|--save-to|--to=<Str>                                write fit settings and results to json or sav file
    -n|--Num|--npts[=Int]                                    number of points in the theoretical curves [default: 100]
    --ac|--AC|--AuxCode|--auxcode|--aux-code=<Str>           additional auxiliar code [default: '']
    --zt|--zip-to=<Str>                                      zip work-folder to zipfile
    --inp|--no-plot                                          no plot in onefit random ...
    --hybrid                                                 hybrid fit [default: False]
    --rc|--rchi2|--reduced-chi2                              recalculate parameters' errors for a Chi2 ~ Number of Degrees of Freedom [default: False]
    --eb|--error-bars|--errorbars                            set errorbars [default: False]
    --mp4|--mpeg4                                            create a mpeg4 video with All.pdf [default: False]
    --parallel                                               set Env ONEFITE_NO_PARALLEL (default = 0) [default: True]
    --workers[=Int]                                          set ENV ONEFITE_WORKERS (default = 0) [default: 0]
    --ro|--remove-outliers=<Str>                             remove outliers (ex: --ro=2 or --ro=2..5, or --ro=1,3,5) [default: '']
    --pc|--prco|--print-cols|--print-columns|--cols=<Str>    select fit-results columns by number or name or name+1 or name-1 [default: '']
    --pco|--peco|--pearson-correlation|--R2                  Pearson correlation coefficient (quality coefficient) [default: False]
    -e|--export                                              extract data from json file and create alias for the json fitting function [default: False]
    -i|--individual                                          forces fit to be individual [default: False]
    -p|--post-install                                        post-install [default: True]
    <input-file>     name of the file to be converted (.sav or .json)
    <output-file>    name of the converted file (.sav or .json)
    fit-methods: simp scan min minos

    <function> syntax alternatives:
    /"y(x,a,b,c)=a + b*x + c*pow(x,2.0)"
    "y(t,A,lambda: 0.1)=A*exp(-lambda*t)" 0.1 is the initial value to start mimimization
    "y (t,A,B=2,lambda)=A * (1 - B*exp(-lambda*t) )" B is fixed with value 2
    "y( t,A, lambda: [0;0.5])=A*exp(-lambda*t)" [lambda is limited to range 0 to 0.
    "R1(f:[0;1e9], A, tau[0<1e-9])=A*tau/(1+pow(2*pi*f*tau,2.0)" f is limite to positive values up to 1e9 and tau also
    "y[-1;1](x:[0<1],a,b,c)=a + b*x + c*x*x" theoretical curves generated for x is limited to range 0 to 1; in the plot y axis will be limited to range -1 to 1
    "f(x,p1: 2, p2=0.3, p3[5<10]) :[-5;5]=p1*exp(-p2*x)*sin(2*pi*x/p3)" p1 is initialized to 1, p2 is fixed to 2 p3 will be in the range 5 to 10
```

### Examples

```bash
	onefite fit "Mz(t[1e-3<20],M0,Mi,T1[1e-3<10])=Mi\+(M0-Mi)*exp(-t/T1)" file.hdf5 --autox --logx --autoy
```

```bash
	onefite fit "Mz(t[1e-3<20],M0,Mi,T1[1e-3<10])=Mi\+(M0-Mi)*exp(-t/T1)" file.hdf5 --autox --logx --autoy --define-alias=1exp

```bash
	onefite fit "Mz(t[1e-3<20],M0,Mi,T1[1e-3<10])=Mi\+(M0-Mi)*exp(-t/T1)" file.hdf5 --logx  "--#Mz[-2<2]" --#T1:0.1...

```bash
	onefite fit "Mz(t[1e-3<20],M0_,Mi_,c[0.5<1],T11_[1e-3<3],T12_[1e-3<3])=Mi_\+c*(M0_-Mi_)*exp(-t/T11_)+(1-c)*(M0_-Mi_)*exp(-t/T12_)" file.hdf5 --autox --logx --autoy 
```
```bash
	onefite fit "Mz(t[1e-3<20],M0_,Mi_,c[0.5<1],T11_[1e-3<3],T12_[1e-3<3])=Mi_\+c*(M0_-Mi_)*exp(-t/T11_)+(1-c)*(M0_-Mi_)*exp(-t/T12_)" file.hdf5 --autox --logx --autoy --global
```

```bash
	onefite fit "Mz(t[1e-3<20],M0_,Mi_,c[0.5<1],T11_[1e-3<3],T12_[1e-3<3])=Mi_\+c*(M0_-Mi_)*exp(-t/T11_)+(1-c)*(M0_-Mi_)*exp(-t/T12_)" file.hdf5 --autox --logx --autoy --hybrid --mp4 --fit-methods="simp scan min"
```

```bash
	onefite fit "#2exp" file.hdf5 --autox --logx --autoy --hybrid --mp4 --fit-methods="simp scan min" --save-to=file.json
```

```bash
	onefite fit file.json --autox --logx --autoy --hybrid --mp4 --fit-methods="simp scan min" --save-to=file.json
```

```bash
	onefite fit "alias: 1exp" file.hdf5 --autox --logx --autoy
```

```bash
	onefite fit "a: 1exp" file.hdf5 --autox --logx --autoy
```

```bash
	onefite fit "a: 1exp" file.txt --autox --logx --autoy
```

```bash
	onefite fit "a: 2BPP" *.dat --autox --logx --autoy --logy
```

```bash
	onefite fit "a: 1BPP" file.zip --autox --logx --autoy --logy
```
	
```bash
	onefite fit "a: 1BPP" file.zip --autox --logx --autoy --logy --define-alias=1BPP
```

```bash
	onefite fit "#1exp" file.txt --autox --logx --autoy
```

```bash
	onefite fit \#1BPP file.zip --autox --logx --autoy --logy --define-alias=1BPP
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat
```

```bash
	onefite fit "y[0<1](x[-1<1],a:1,b=2) = a + b*x" file.dat
```

```bash
	onefite fit "y[0<1](x[-1<1],a:1[-1<1],b:0.1[-2<2]) = a + b*x" file.dat "--#a=0.0..." --#x"[0<2]"
```
	
```bash
	onefite fit "y[0<1](x[-1<1],a:1[-1<1],b:0.1[-2<2]) = a + b*x" file.dat "--#a=0.0[0<2]" "--#y[0<2]"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="1.0"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="x10/100"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="10x"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="1%"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="std"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="standard deviation"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="std split at 5"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="1% avg"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="1% average"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="10% avg split at 10.5" --cols="a,b,b+1"
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="10% avg split at 10.5" --remove-outliers=1
```

```bash
	onefite fit \#1BPP file.zip --autox --logx --autoy --logy --define-alias=1BPP --reduced-chi2
```

```bash
	onefite fit \#1BPP file.zip --autox --logx --autoy --logy --define-alias=1BPP --archive
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="10% avg split at 10.5" --remove-outliers=1 --ar
```

```bash
	onefite fit "y(x,global:1[-1<1],individual_=1[-2<2].MIXED=1) = global + individual_*x" files*.dat --no-parallel
```

```bash
	onefite fit "y(x,global:1[-1<1],individual_=1[-2<2].MIXED=1) = global + individual_*x" files*.dat
```

```bash
	onefite fit "y(x,global:1[-1<1],individual_=1[-2<2].MIXED=1) = global + individual_*x" files*.dat --workers=2
```

```bash
	onefite fit "y(x,a,b) = a + b*x" file.dat --set-err="10% avg split at 10.5" --remove-outliers=1 --ar
```

```bash
	onefite archive --fit last
```

```bash
	onefite archive -f last-1 --rchi2
```

```bash
	onefite log -f 2 --ro 1
```

```bash
	onefite log --fit=5 --save-to=trash.json
```



