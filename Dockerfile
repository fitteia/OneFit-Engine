FROM debian:trixie

ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
	vim \
    sudo \
    git \
    curl \
    shellinabox \
    build-essential \
    libreadline-dev \
    libssl-dev \
    raku \
    zsh \
    libperl-dev \
    swig \
    gcc \
    dpkg-dev \
   	gfortran \
   	grace \
   	imagemagick \
   	texlive-font-utils \
   	ghostscript \
   	libcgi-pm-perl \
   	zip \
   	pdftk \
    jq \
   	hdf5-tools \
    man-db \
    tasksel \ 
    && apt-get clean

# Create user 'ofe' with password and sudo access
RUN useradd -ms /bin/bash ofe && \
    echo 'ofe:ofe' | chpasswd && \
    usermod -aG sudo ofe 

# Switch to user 'ofe' to perform user-specific actions
USER ofe

# Create target directories and clone the repo
RUN mkdir /home/ofe/public_html &&\
	mkdir -p /home/ofe/.local && \
	cd /home/ofe/.local && \
	mkdir -p bin etc log && \
    git clone https://github.com/fitteia/OneFit-Engine.git /home/ofe/.local/OneFit-Engine 
 
RUN cd /home/ofe/.local/OneFit-Engine && echo "ofe\nY\nY\nY" | sudo -S ./INSTALL --docker

# Expose SSH (2222), Apache (8142), ShellInABox (8100)
EXPOSE 8142  

# Start services on container launch
CMD onefite
