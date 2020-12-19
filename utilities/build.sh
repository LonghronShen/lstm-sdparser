#!/bin/bash

export WITH_CUDA=${1:-off}
export CUDA_ARCH=${2:-500}

mkdir -p build

cd build
cmake -DWITH_CUDA="${WITH_CUDA}" -DCUDA_ARCH="${CUDA_ARCH}" ..

cmake --build . -j$(nproc)