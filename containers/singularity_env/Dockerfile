FROM ubuntu:jammy

USER root
RUN mkdir /app
WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    uuid-dev \
    libgpgme11-dev \
    squashfs-tools \
    libseccomp-dev \
    pkg-config \
    crun \
    cryptsetup-bin \
    uidmap \
    ca-certificates

ADD http://pki.jlab.org/JLabCA.crt /usr/local/share/ca-certificates/JLabCA.crt
RUN chmod 644 /usr/local/share/ca-certificates/JLabCA.crt && \
    update-ca-certificates && \
    echo "check_certificate = off" >> ~/.wgetrc

ADD https://github.com/sylabs/singularity/releases/download/v3.11.3/singularity-ce_3.11.3-jammy_amd64.deb singularity.deb
RUN dpkg -i singularity.deb

CMD /bin/bash

