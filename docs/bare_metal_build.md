# Build PHASM through Bash Scripts


#### Complete all the installation steps at once
```bash
#scl enable devtoolset-11   # Make sure you are using a recent compiler
./install.sh

# To Run a PHASM example: 
export LD_LIBRARY_PATH=$DEPS/libtorch/lib:$LD_LIBRARY_PATH

# Run the PDE solver example and dump captured data to CSV
PHASM_CALL_MODE=CaptureAndDump install/bin/phasm-example-pdesolver
```

OR if you would prefer to do it manually, follow the steps below

```bash

# Install PyTorch, Intel Pin, and JANA2 dependencies
./download_deps.sh
export DEPS=`pwd`/deps

# Build 
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="$DEPS/libtorch;$DEPS/JANA2/install" -DLIBDWARF_DIR="$DEPS/libdwarf-0.3.4/installdir" -DPIN_ROOT="$DEPS/pin" ..
make install

# To run one of the examples:
export LD_LIBRARY_PATH=$DEPS/libtorch/lib:$LD_LIBRARY_PATH

# Run the PDE solver example and dump captured data to CSV
PHASM_CALL_MODE=CaptureAndDump install/bin/phasm-example-pdesolver

# Run vacuum tool against the example target program
install/bin/phasm-memtrace-pin install/bin/phasm-example-memtrace
```