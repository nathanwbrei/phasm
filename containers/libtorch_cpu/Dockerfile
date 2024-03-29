# Dockerfile to solve basic PHASM dev environment.
#
# Updated by Cissie July-26-2023.
# To see the progress more clearly: `docker build --progress plain`
# Build to a certain stage: `docker build --target <target_string>`
# Use a different spack env: `docker build --build-arg="SPACK_ENV_NAME=julia"`

################################################################
# Phase 1: call spack to configure env.
# @ARG SPACK_ENV_NAME is used to select configuration files.
#
# Ref: https://spack.readthedocs.io/en/latest/containers.html
################################################################

FROM spack/ubuntu-jammy:latest as builder

ARG SPACK_ENV_NAME=basic
COPY spack_env/$SPACK_ENV_NAME.yaml /opt/spack-environment/spack.yaml

# Install the software, remove unnecessary deps
WORKDIR /opt/spack-environment
RUN spack env activate . && spack install --fail-fast && spack gc -y

# Strip all the binaries
RUN find -L /opt/view/* -type f -exec readlink -f '{}' \; | \
    xargs file -i | \
    grep 'charset=binary' | \
    grep 'x-executable\|x-archive\|x-sharedlib' | \
    awk -F: '{print $1}' | xargs strip -s

# Modifications to the environment that are necessary to run
RUN spack env activate --sh -d . >> /etc/profile.d/phasm_spack_environment.sh

################################################################
# Phase 2: install libtorch and other dependencies.
################################################################
FROM ubuntu:jammy as base

# Set up env by Spack installation
COPY --from=builder /opt/spack-environment /opt/spack-environment
COPY --from=builder /opt/software /opt/software
COPY --from=builder /opt/view /opt/view
COPY --from=builder /etc/profile.d/phasm_spack_environment.sh /etc/profile.d/phasm_spack_environment.sh
ENV PATH="$PATH:/opt/view/bin"

# Install basic development environment
RUN apt update -y && \
    apt install -y gdb unzip valgrind wget vim libasan6 less exa bat git zlib1g-dev build-essential

# Fix JLab certificates
ADD http://pki.jlab.org/JLabCA.crt /usr/local/share/ca-certificates/JLabCA.crt
RUN chmod 644 /usr/local/share/ca-certificates/JLabCA.crt && \
    update-ca-certificates
    
RUN echo "check_certificate = off" >> ~/.wgetrc

RUN mkdir /deps
WORKDIR /deps

# Install libtorch-cpu. Locked to Libtorch-cpu 1.12.
# Url updated on July-17 2023 by xmei@jlab.org.
ARG TORCH_URL="https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.12.0%2Bcpu.zip"
RUN wget ${TORCH_URL} \
    && mv libtorch-*.zip libtorch.zip \
    && unzip libtorch.zip \
    && rm libtorch.zip

######################################################################
# Phase 3: install JANA v2.0.6 and set up env.
######################################################################
# Install JANA (Do this last because we are more likely to upgrade!)
RUN git clone -b v2.0.6 http://github.com/JeffersonLab/JANA2 \
    && mkdir JANA2/install \
    && mkdir JANA2/build \
    && cd JANA2/build \
    && cmake .. -DCMAKE_INSTALL_PREFIX=/deps/JANA2/install \
    && make -j8 install \
    && cd .. \
    && rm -rf build/

ENV JANA_HOME=/deps/JANA2/install
ENV PATH="$PATH:/deps/JANA2/install/bin"
ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/deps/libtorch/lib:/deps/JANA2/install/plugins"

ENV DEPS=/deps
RUN mkdir /app
WORKDIR /app
CMD /bin/bash
