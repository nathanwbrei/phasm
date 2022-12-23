FROM nvidia/cuda:11.8.0-cudnn8-devel-ubuntu22.04

USER root
RUN mkdir /app
WORKDIR /app

# Install basic development environment
RUN apt update -y \
 && apt install -y gdb valgrind cmake wget unzip vim libasan6 less exa bat git zlib1g-dev

# Install libtorch
# This is still a cpu-only libtorch
ADD "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.11.0%2Bcpu.zip" libtorch.zip
RUN unzip libtorch.zip

# Install libdwarf
ADD "https://github.com/davea42/libdwarf-code/releases/download/v0.3.4/libdwarf-0.3.4.tar.xz" libdwarf.tar.xz
RUN tar -xf libdwarf.tar.xz \
 && mv libdwarf-0.3.4 libdwarf \
 && mkdir libdwarf/build \
 && mkdir libdwarf/installdir \
 && cd libdwarf/build \
 && cmake .. -DCMAKE_INSTALL_PREFIX=/app/libdwarf/installdir \
 && make install

# Install Intel PIN
ADD "https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.22-98547-g7a303a835-gcc-linux.tar.gz" pin.tar.gz
RUN tar -xf pin.tar.gz \
 && mv pin-3.22-98547-g7a303a835-gcc-linux pin

# Install JANA (Do this last because we are more likely to upgrade!)
RUN git clone -b v2.0.6 http://github.com/JeffersonLab/JANA2 \
 && mkdir JANA2/install \
 && mkdir JANA2/build \
 && cd JANA2/build \
 && cmake .. -DCMAKE_INSTALL_PREFIX=/app/JANA2/install \
 && make -j8 install

ENV JANA_HOME=/app/JANA2/install
ENV PATH="$PATH:/app/JANA2/install/bin"
ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/app/JANA2/install/plugins"
ENV PIN_ROOT="/app/pin"

CMD /bin/bash
