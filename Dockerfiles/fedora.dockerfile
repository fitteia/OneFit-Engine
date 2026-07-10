FROM fedora:latest

ARG HOST_UID=1000
ARG HOST_GID=1000

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN dnf -y update && \
    dnf -y install \
        sudo \
        passwd \
        shadow-utils \
        git \
        curl \
        wget \
        ca-certificates \
        vim \
        less \
        findutils \
        procps-ng \
        iproute \
        bash \
        make \
        gcc \
        gcc-gfortran \
        gnuplot \
        grace \
        jq \
        unzip \
        zip \
        perl-devel && \
    dnf clean all

# Create an OFE user with the same numeric UID/GID as the host user.
RUN groupadd -o -g ${HOST_GID} ofe && \
    useradd  -o -m -u ${HOST_UID} -g ${HOST_GID} -s /bin/bash ofe && \
    usermod -aG wheel ofe && \
    echo "%wheel ALL=(ALL) NOPASSWD: ALL" >/etc/sudoers.d/wheel && \
    chmod 440 /etc/sudoers.d/wheel && \
    mkdir -p /home/ofe/.local /home/ofe/work && \
    chown -R ${HOST_UID}:${HOST_GID} /home/ofe

EXPOSE 8142

USER ofe
WORKDIR /home/ofe/work

CMD ["/bin/bash"]
