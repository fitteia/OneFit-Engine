FROM debian:latest

ARG HOST_UID=1000
ARG HOST_GID=1000

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN apt-get update && \
    apt-get -y install --no-install-recommends \
        sudo \
        passwd \
        git \
        curl \
        wget \
        ca-certificates \
        vim \
        less \
        findutils \
        procps \
        iproute2 \
        bash \
        make \
        gcc \
        gfortran \
        raku \
        gnuplot \
        grace \
        jq \
        unzip \
        zip && \
    rm -rf /var/lib/apt/lists/*

# Create an OFE user with the same numeric UID/GID as the host user.
# The -o option allows macOS group IDs such as 20, which may already exist in Debian.
RUN groupadd -o -g ${HOST_GID} ofe && \
    useradd  -o -m -u ${HOST_UID} -g ${HOST_GID} -s /bin/bash ofe && \
    echo "ofe:ofe" | chpasswd && \
    usermod -aG sudo ofe && \
    echo "%sudo ALL=(ALL) NOPASSWD: ALL" >/etc/sudoers.d/sudo && \
    chmod 440 /etc/sudoers.d/sudo && \
    mkdir -p /home/ofe/.local && \
	mkdir -p /home/ofe/work && \
    chown -R ${HOST_UID}:${HOST_GID} /home/ofe

EXPOSE 8142

USER ofe
WORKDIR /home/ofe/work

CMD ["/bin/bash"]
